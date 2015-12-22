#include "connection.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <errno.h>
#include <string.h>
#include <string>
#include "network.h"
#include "log.h"
#include "request.h"
#include "response.h"
#include "fdevent.h"
#include "utils.h"


namespace ustc_beyond {
namespace tril {
//public

bool Connection::ConnectionReset() {
    this->is_keepalived = 0;
    this->is_writable = 0;
    this->is_readable = 0;
    request = new Request();
    request->RequestInit();
    response = new Response();
    response->ResponseInit();
    return true;
}


void Connection::ConnectionAdjustQueue(std::deque<std::string> queue, std::string newdata) {
    queue.clear();
    queue.push_back(newdata);
}

std::string Connection::ConnectionMergeQueue(std::deque<std::string> queue) {
    std::string res = "";
    for(unsigned i = 0; i < queue.size(); i++) {
        res += queue[i];
    }
    return res;
}

bool Connection::ConnectionReadFromFd() {
    //read and detect \r\n\r\n
    int len;
    char buf[READ_BUFFER_SIZE] = {'\0'};
    len = read(fd, buf, READ_BUFFER_SIZE);

    std::cout <<fd << " read len is " << len << std::endl;
    if (len < 0) {
        ConnectionSetReadable(false);

        ConnectionSetState(CON_STATE_ERROR);
        if (errno == EINTR || errno == EAGAIN)
            /* we have been interrupted before we could read */
            //con->is_readable = 1;
            return false;
    } else if (len == 0) {
        //con->is_readable = 0;
        /* the other end close the connection -> KEEP-ALIVE */

        /* pipelining */

        ConnectionSetReadable(false);
        return true;
    } else if ((size_t)len <= READ_BUFFER_SIZE) {

        ConnectionSetReadable(false);
        read_queue.push_back(std::string(buf));
        std::cout << buf << std::endl;
        //con->is_readable = 0;
    }

    //check \r\n\r\n
    std::string head = ConnectionMergeQueue(this->read_queue);
    size_t blank_line = head.find("\r\n\r\n");
    if(blank_line != string::npos) {
        //we get all header
        request->SetRequestHead(head.substr(0, blank_line));
//        std::string remain = head.substr(blank_line + 4, head.size() - blank_line - 4);
        ConnectionAdjustQueue(this->read_queue, head.substr(blank_line + 4, head.size() - blank_line - 4));
//        ConnectionAdjustQueue(this->read_queue, remain);
        ConnectionSetState(CON_STATE_REQUEST_END);
    }
    else {
        ConnectionSetState(CON_STATE_READ);
        //continue read
        std::cout <<  "continue read" << std::endl;
    }

    return true;
}

int Connection::ConnectionParseRequest() {

    if(!request->HttpRequestParse()) {
        std::cout <<  "parse request failed!" << std::endl;
    }

    if(request->HttpRequestIsPost())
        return 1;
    else
        return 0;
}

int Connection::ConnectionGenerateResponse() {
    // query path first
    std::string url = request->GetHeadValue("URI");
    //std::cout << url << std::endl;
    std::string htdoc = config->GetConfigValue("htdoc");
    std::string tmp_url;
    std::string file_content;

    response->SetHeadValue("Protocol", request->GetHeadValue("Protocol"));

    response->SetHeadValue("Server", "tril 1.0");
    response->SetHeadValue("Date", "Wed, 16 Dec 2015 13:06:23 GMT");
    response->SetHeadValue("Content-Type", "text/html;charset=utf-8");

    if(StringEndWith(url, "/")) {
        tmp_url = url + "index.htm";
        if(!FileExist(htdoc + tmp_url)) {
            tmp_url = url + "index.html";
            if(!FileExist(htdoc + tmp_url)) {
                response->SetHeadValue("Status", "404");
                return HANDLER_FINISHED;
            }
        }
        url = tmp_url;
    }

    // set header
    std::string real_path = htdoc + url;
    std::cout <<"real_path is" << real_path << std::endl;
    if(!FileExist(real_path)) {
        response->SetHeadValue("Status", "404");
        file_content = "<html<body><h1>Sorry, 404 Not Found</h1></body></html>"; 
        response->SetHeadValue("Content-Length", NumberToString<int>(file_content.size()));
        //std::cout << file_content << std::endl;
        response->SetResponseEntity(file_content);
        return HANDLER_FINISHED;
    }

    if(StringEndWith(real_path, "htm") || StringEndWith(real_path, "html"))
        response->SetHeadValue("Content-Type", "text/html;charset=utf-8");
    else
        response->SetHeadValue("Content-Type", "text/plain;charset=utf-8");

    response->SetHeadValue("Status", "200");

    std::cout << real_path << std::endl;
    file_content = GetFileContent(real_path);

    response->SetHeadValue("Content-Length", NumberToString<int>(file_content.size()));
//    std::cout << file_content << std::endl;
    response->SetResponseEntity(file_content);
    return HANDLER_FINISHED;
}

bool Connection::ConnectionWriteToFd() {
    //生成response
    response->GenerateFinalResponse();
    // set content
    write_queue.push_back(response->GetFinalResponse());

    int len;
    std::string content = write_queue[0];
    len = write(fd, content.c_str(), content.size());
    if(len >= 0) {
        if(len < content.size()) { //not finished
            write_queue.clear();
            write_queue.push_back(content.substr(len, content.size() - len));
            ConnectionSetState(CON_STATE_WRITE);
        }
        else { //we have finished
            std::cout << "Finished Sending The File " << len << " and "<< content << std::endl;
            ConnectionSetState(CON_STATE_RESPONSE_END);
        }
    }
    else {
        switch(errno) {
        case EAGAIN :
        case EINTR :
            std::cout << "write fd error" << std::endl;
            break;
        }
    }
    return true;
}

bool Connection::ConnectionClose() {
    shutdown(fd, SHUT_WR);
    request->RequestFree();
    delete request;
    response->ResponseFree();
    delete response;
    read_queue.clear();
    write_queue.clear();
    return true;
}

bool Connection::ConnectionStateMachine(Server* srv) {
    int done = 0, r;
    Fdevent* ev = srv->GetFdevent();


    while (done == 0) {
        size_t ostate = this->state;

        switch (this->state) {
        case CON_STATE_REQUEST_START: /* transient */

            ConnectionSetState(CON_STATE_READ);

            break;
        case CON_STATE_REQUEST_END: /* transient */
            if (ConnectionParseRequest()) {
                /* we have to read some data from the POST request */

                ConnectionSetState(CON_STATE_READ_POST);

                break;
            }

            ConnectionSetState(CON_STATE_HANDLE_REQUEST);

            break;
        case CON_STATE_HANDLE_REQUEST:

            switch (r = ConnectionGenerateResponse()) {
            case HANDLER_FINISHED:
                std::cout << "Generate Response Success!" << std::endl;
                /* we have something to send, go on */
                //ConnectionSetState(CON_STATE_RESPONSE_START);
                ConnectionSetState(CON_STATE_WRITE);
                done = 1;
                break;
            case HANDLER_ERROR:
                /* something went wrong */
                ConnectionSetState(CON_STATE_ERROR);
                break;
            default:
                break;
            }

            break;
        /*        case CON_STATE_RESPONSE_START:
                    if (-1 == ConnectionGenerateResponse()) {
                        ConnectionSetState(CON_STATE_ERROR);

                        break;
                    }

                    ConnectionSetState(CON_STATE_WRITE);
                    break;
                    */
        case CON_STATE_RESPONSE_END: /* transient */
            /* log the request */

            if (this->is_keepalived) {
                ConnectionSetState(CON_STATE_REQUEST_START);

            } else {
                ConnectionSetState(CON_STATE_CLOSE);
                std::cout << "Start to close fd" << std::endl;
            }
            break;
        case CON_STATE_CLOSE:
            ConnectionClose();
            done = 1;
            /* we have to do the linger_on_close stuff regardless
             * of con->keep_alive; even non-keepalive sockets may
             * still have unread data, and closing before reading
             * it will make the client not see all our output.
             */

            break;
        case CON_STATE_CONNECT:
            ConnectionSetState(CON_STATE_READ);
            done = 1;
            break;
        case CON_STATE_READ_POST:
        case CON_STATE_READ:
            if(!ConnectionReadFromFd()) {
                srv->log.Log(kInfo, "read from fd Nothing");
                ConnectionSetState(CON_STATE_CLOSE);
            }
            //set state inside
            //ConnectionSetState(CON_STATE_REQUEST_END);
            break;
        case CON_STATE_WRITE:
            //if (!this->ConnectionWriteQueueEmpty()) {
            if (false == ConnectionWriteToFd()) {
                ConnectionSetState(CON_STATE_ERROR);
            }
            //}
            break;
        case CON_STATE_ERROR: /* transient */
            break;
        }

        if (done == -1) {
            done = 0;
        } else if (ostate == this->state) {
            //std::cout << "read set" << std::endl;
            done = 1;
        }
    }

    switch(state) {
    case CON_STATE_CONNECT:
    case CON_STATE_READ_POST:
    case CON_STATE_READ:
    case CON_STATE_CLOSE:
        //ev->FdeventEventDel(fd);
        ev->FdeventEventSet(this->fd, FDEVENT_IN);
        std::cout << "read fd "<< fd << std::endl;
        break;
    case CON_STATE_WRITE:
        //ev->FdeventEventDel(fd);
        ev->FdeventEventSet(this->fd, FDEVENT_OUT);
        /* request write-fdevent only if we really need it
         * - if we have data to write
         * - if the socket is not writable yet
         */
        /*if (!chunkqueue_is_empty(con->write_queue) &&
            (con->is_writable == 0) &&
            (con->traffic_limit_reached == 0)) {
        	fdevent_event_set(srv->ev, &(con->fde_ndx), con->fd, FDEVENT_OUT);
        } else {
        	fdevent_event_del(srv->ev, &(con->fde_ndx), con->fd);
        }*/
        break;
    default:
        //fdevent_event_del(srv->ev, &(con->fde_ndx), con->fd);
        break;
    }

    return true;
}

}
}




