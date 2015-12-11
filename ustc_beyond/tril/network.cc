#include "network.h"
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include "iostream"
#include "fdevent.h"
#include "connection.h"

namespace ustc_beyond {
namespace tril {

handler_t NetworkHandleFunc::FdeventHandler(Server* srv, void* ctx, int revents) {
	Connection *con;
    Network* net = (Network*)ctx;
	int loops = 0;


	if (0 == (revents & FDEVENT_IN)) {
        srv->log.Log(kError, "Get network event error");
		return HANDLER_ERROR;
	}

	/* accept()s at most 100 connections directly
	 *
	 * we jump out after 100 to give the waiting connections a chance */
	for (loops = 0; loops < 100 && NULL != (con = ConnectionAccept(net, srv->GetNetwork()->GetSockFd())); loops++) {
//		handler_t r;

//		connection_state_machine(srv, con);

	}
	return HANDLER_GO_ON;
}

bool Network::NetworkInit(Server* srv) {

    //HandleFunc* handler_func = NetworkCreateHandleFunc();
    
    network_handle_func = new NetworkHandleFunc();
    sock_fd = -1;
    
    std::string host_port = srv->GetConfigValue("port");
    std::string host_name = srv->GetConfigValue("hostname");

    unsigned int port = StringToNumber<int>(host_port);
    if(port == 0) {
        srv->log.Log(kError, "GetPortError");
        return false;
    }

    if(host_name == "") {
        srv->log.Log(kError, "Get host name error");
        return false;
    }


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
    if(!ev->FdeventRegister(sock_fd, network_handle_func, NULL)){
        srv->log.Log(kError, "Register event error");
        return false;
    }            

    if(!ev->FdeventEventSet(sock_fd, FDEVENT_IN)){
        srv->log.Log(kError, "Set event error");
        return false;
    }            
    return true;
}


bool Network::NetworkClose() {
    delete network_handle_func; 
    //we should delete connections here?
    //delete con; 
    return true;
}

Connection * NetworkHandleFunc::ConnectionAccept(Network* net, int fd) {

	int cnt;
	sock_addr cnt_addr;
	socklen_t cnt_len;
	/* accept it and register the fd */

	/**
	 * check if we can still open a new connections
	 *
	 * see #1216
	 */


	cnt_len = sizeof(cnt_addr);

	if (-1 == (cnt = accept(fd, (struct sockaddr *) &cnt_addr, &cnt_len))) {
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
		return NULL;
	} else {
		Connection *con;


		/* ok, we have the connection, register it */

//		con = connections_get_new_connection(net);

/*		con->fd = cnt;
		con->fde_ndx = -1;

		fdevent_register(srv->ev, con->fd, connection_handle_fdevent, con);

		connection_set_state(srv, con, CON_STATE_REQUEST_START);

		con->connection_start = srv->cur_ts;
		con->dst_addr = cnt_addr;
		buffer_copy_string(con->dst_addr_buf, inet_ntop_cache_get_ip(srv, &(con->dst_addr)));
		con->srv_socket = srv_socket;

		if (-1 == (fdevent_fcntl_set(srv->ev, con->fd))) {
			log_error_write(srv, __FILE__, __LINE__, "ss", "fcntl failed: ", strerror(errno));
			return NULL;
		}
        */
        std::cout << "Success" << std::endl;
		return con;
	}
}
}
}




