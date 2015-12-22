#include "network.h"
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include "fdevent.h"
#include "connection.h"
#include "configure.h"

namespace ustc_beyond {
namespace tril {

handler_t ConnectionHandleFdevent::FdeventHandler(Server* srv, void* ctx, int revents) {
    std::cout << "this is called" << std::endl;
    Fdevent* ev = srv->GetFdevent();
    Connection *con = (Connection*)ctx;


    if (revents & FDEVENT_IN) {
        con->ConnectionSetReadable(true);
    }
    if (revents & FDEVENT_OUT) {
        con->ConnectionSetWriteable(true);
    }


    if (revents & ~(FDEVENT_IN | FDEVENT_OUT)) {
        /* looks like an error */

        con->ConnectionSetState(CON_STATE_ERROR);

    }

    /*   if (con->ConnectionGetState() == CON_STATE_READ ||
               con->ConnectionGetState() == CON_STATE_READ_POST) {
           if(!con->ConnectionReadFromFd()) {
               srv->log.Log(kError, "read from fd error");
           };
       }

       if (con->ConnectionGetState() == CON_STATE_WRITE &&
               !con->ConnectionWriteQueueEmpty() &&
               con->ConnectionGetWriteable()) {

           if (!con->ConnectionWriteToFd()) {
               con->ConnectionSetState(CON_STATE_ERROR);
               srv->log.Log(kError, "handle write failed.");
           }
       }
       */

    if (con->ConnectionGetState() == CON_STATE_CLOSE) {
        /* flush the read buffers */
        int len;
        char buf[1024];

        len = read(con->ConnectionGetFd(), buf, sizeof(buf));
        if (len == 0 || (len < 0 && errno != EAGAIN && errno != EINTR) ) {
            //con->close_timeout_ts = srv->cur_ts - (HTTP_LINGER_TIMEOUT+1);
        }
        ev->FdeventEventDel(con->ConnectionGetFd());
        ev->FdeventUnregister(con->ConnectionGetFd());
        std::cout <<  "If close?" << std::endl;

        return HANDLER_FINISHED;

    }
    //enter the state machine
    con->ConnectionStateMachine(srv);

    return HANDLER_FINISHED;
}

handler_t NetworkHandleFunc::FdeventHandler(Server* srv, void* ctx, int revents) {
    Connection *con;
//    Network* net = (Network*)ctx;
    int loops = 0;


    if (0 == (revents & FDEVENT_IN)) {
        srv->log.Log(kError, "Get network event error");
        return HANDLER_ERROR;
    }

    std::cout << "a new connection coming" << std::endl;
    /* accept()s at most 100 connections directly
     *
     * we jump out after 100 to give the waiting connections a chance */
    for (loops = 0; loops < 100 && NULL != (con = ConnectionAccept(srv)); loops++) {
//		handler_t r;

        con->ConnectionStateMachine(srv);

    }
    return HANDLER_GO_ON;
}

bool Network::NetworkInit(Server* srv) {

    //HandleFunc* handler_func = NetworkCreateHandleFunc();

    network_handle_func = new NetworkHandleFunc();
    connect_handle_func = new ConnectionHandleFdevent();
    config = srv->GetConfig();
    sock_fd = -1;

    std::string host_port = config->GetConfigValue("port");
    std::string host_name = config->GetConfigValue("hostname");

    unsigned int port = StringToNumber<int>(host_port);
    if(port == 0) {
        srv->log.Log(kError, "GetPortError");
        return false;
    }

    /*   if(host_name == "") {
           srv->log.Log(kInfo, "Get host name error");
           //return false;
       }
       */


    int val;
    sock_addr addr;
    sock_addr_in addr_in;
    sock_addr_in6 addr_in6;
    socklen_t addr_len = sizeof(struct sockaddr_in);


    if (host_name[0] == '[' && host_name[host_name.size() - 1]  == ']') {
        use_ipv6 = 1;
    }

    if (use_ipv6) {
        addr.sa_family = AF_INET6;

        if (-1 == (sock_fd = socket(addr.sa_family, SOCK_STREAM, IPPROTO_TCP))) {
            srv->log.Log(kError, "CreateSocketFailed");
            return false;
        }
        use_ipv6 = 1;
    }

    if (sock_fd == -1) {
        addr.sa_family = AF_INET;
        if (-1 == (sock_fd = socket(addr.sa_family, SOCK_STREAM, IPPROTO_TCP))) {
            srv->log.Log(kError, "CreateSocketFailed");
            return false;
        }
    }

    /* set FD_CLOEXEC now, fdevent_fcntl_set is called later; needed for pipe-logger forks */
    fcntl(sock_fd, F_SETFD, FD_CLOEXEC);


    val = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        srv->log.Log(kError, "SetSocketOptionFailed");
        return false;
    }

    switch(addr.sa_family) {
    case AF_INET6:
        addr_in6.sin6_family = AF_INET6;
        if (host_name == "") {
            addr_in6.sin6_addr = in6addr_any;
        } else {
            struct addrinfo hints, *res;
            int r;
            val = 1;
            if (-1 == setsockopt(sock_fd, IPPROTO_IPV6, IPV6_V6ONLY, &val, sizeof(val))) {
                srv->log.Log(kError, "Set IPV6 option failed");
                return false;
            }

            memset(&hints, 0, sizeof(hints));

            hints.ai_family   = AF_INET6;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            if (0 != (r = getaddrinfo(host_name.c_str(), NULL, &hints, &res))) {
                srv->log.Log(kError, "Getaddrinfo error %s", gai_strerror(r));
                return false;
            }

            memcpy(&(addr_in6.sin6_addr.s6_addr), res->ai_addr, res->ai_addrlen);

            freeaddrinfo(res);
        }
        addr_in6.sin6_port = htons(port);
        addr_len = sizeof(struct sockaddr_in6);
        break;

    case AF_INET:
        addr_in.sin_family = AF_INET;
        if (host_name == "") {
            addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
        } else {
            struct hostent *he = gethostbyname(host_name.c_str());

            if (NULL == he) {
                srv->log.Log(kError, "Getaddrinfo error");
                return false;
            }

            if (he->h_addrtype != AF_INET) {
                srv->log.Log(kError, "Type not AF_INET");
                return false;
            }

            if (he->h_length != sizeof(struct in_addr)) {
                srv->log.Log(kError, "Addr length error");
                return false;
            }

            memcpy(&(addr_in.sin_addr.s_addr), he->h_addr_list[0], he->h_length);
        }
        addr_in.sin_port = htons(port);

        addr_len = sizeof(struct sockaddr_in);

        break;
    default:
        break;
    }

    if(use_ipv6) {
        memcpy(&(addr), &(addr_in6), sizeof(sock_addr)) ;
    }
    else {
        memcpy(&(addr), &(addr_in), sizeof(sock_addr)) ;
    }
    if (0 != bind(sock_fd, (struct sockaddr *) &(addr), addr_len)) {
        srv->log.Log(kError, "Bind host %s,port %d error", host_name.c_str(), port);
        return false;
    }

    if (-1 == listen(sock_fd, 128 * 8)) {
        srv->log.Log(kError, "Listen failed");
        return false;
    }

    return true;

}

bool Network::NetworkRegisterFdevents(Server* srv) {
    Fdevent* ev = srv->GetFdevent();

    if(!ev->FdeventRegister(sock_fd, network_handle_func, this)) {
        srv->log.Log(kError, "Register event error");
        return false;
    }

    if(!ev->FdeventEventSet(sock_fd, FDEVENT_IN)) {
        srv->log.Log(kError, "Set event error");
        return false;
    }

    if(-1 == ev->FdeventFcntlSet(sock_fd)) {
        srv->log.Log(kError, "Set fcntl error");
        return false;
    }

    return true;
}

Connection* Network::GetNewConnection() {
    Connection* con = new Connection(config);
    con->ConnectionReset();
    return con;
}

void Network::DeleteConnection(Connection* con) {
    delete con;
}

bool Network::NetworkClose() {
    delete network_handle_func;
    delete connect_handle_func;
    //we should delete connections here?
    //delete con;
    return true;
}

Connection * NetworkHandleFunc::ConnectionAccept(Server* srv) {

    int cnt;
    sock_addr cnt_addr;
    socklen_t cnt_len;
    Fdevent* ev = srv->GetFdevent();
    Network* net = srv->GetNetwork();
    /* accept it and register the fd */

    /**
     * check if we can still open a new connections
     *
     * see #1216
     */


    cnt_len = sizeof(cnt_addr);

    if (-1 == (cnt = accept(net->GetSockFd(), (struct sockaddr *) &cnt_addr, &cnt_len))) {
        switch (errno) {
        case EAGAIN:
        case EINTR:
        /* we were stopped _before_ we had a connection */
        case ECONNABORTED: /* this is a FreeBSD thingy */
            /* we were stopped _after_ we had a connection */
            break;
        case EMFILE:
            /* out of fds */
            break;
        default:
            break;
        }
        //std::cout << "connt fd" << cnt << std::endl;
        return NULL;
    } else {
        Connection *con =  net->GetNewConnection();
        con->ConnectionSetFd(cnt);

        ev->FdeventRegister(cnt, net->GetHandleFunc(), con);
        con->ConnectionSetState(CON_STATE_CONNECT);


        ev->FdeventFcntlSet(cnt);//set non block
        /* ok, we have the connection, register it */
        if(net->GetIpv6()) {
            // parse ipv6 address
            char buf[INET6_ADDRSTRLEN + 1];
            inet_ntop(cnt_addr.sa_family, (const void *)&((sock_addr_in6*)&cnt_addr)->sin6_addr, buf, INET6_ADDRSTRLEN);

            con->ConnectionSetClientIp(std::string(buf));
        }
        else {
            //    return &(((struct sockaddr_in*)sa)->sin_addr);
            con->ConnectionSetClientIp(std::string(inet_ntoa(((sock_addr_in*)&cnt_addr)->sin_addr)));
        }

        std::cout << "Success"<< con->ConnectionGetClientIp() << "fd is" << net->GetSockFd() << std::endl;
        return con;
    }
}
}
}






