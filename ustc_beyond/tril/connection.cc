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


namespace ustc_beyond {
namespace tril {
//public

bool Connection::ConnectionReset() {
    this->is_keepalived = 1;
    this->is_writable = 1;
    this->is_readable = 1;
    request = new Request();
    request->request_init();
    response = new Response();
    response->response_init();
    return true;
}

bool Connection::ConnectionReadFromFd() {
    return true;
}

int Connection::ConnectionParseRequest() {
    return 0;
}

int Connection::ConnectionGenerateResponse() {
    return 0;
}

bool Connection::ConnectionWriteToFd() {
    return true;
}

bool Connection::ConnectionClose() {
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
                /* we have something to send, go on */
                ConnectionSetState(CON_STATE_RESPONSE_START);
                break;
            case HANDLER_ERROR:
                /* something went wrong */
                ConnectionSetState(CON_STATE_ERROR);
                break;
            default:
                break;
            }

            break;
        case CON_STATE_RESPONSE_START:
            if (-1 == ConnectionGenerateResponse()) {
                ConnectionSetState(CON_STATE_ERROR);

                break;
            }

            ConnectionSetState(CON_STATE_WRITE);
            break;
        case CON_STATE_RESPONSE_END: /* transient */
            /* log the request */

            if (this->is_keepalived) {
                ConnectionSetState(CON_STATE_REQUEST_START);

                ConnectionSetState(CON_STATE_CLOSE);
            } else {
                ConnectionClose();
            }
            break;
        case CON_STATE_CLOSE:
            /* we have to do the linger_on_close stuff regardless
             * of con->keep_alive; even non-keepalive sockets may
             * still have unread data, and closing before reading
             * it will make the client not see all our output.
             */

            break;
        case CON_STATE_READ_POST:
        case CON_STATE_READ:
            ConnectionReadFromFd();
            break;
        case CON_STATE_WRITE:
            if (!this->write_queue.empty() && this->is_writable) {
                if (false == ConnectionWriteToFd()) {
                    ConnectionSetState(CON_STATE_ERROR);
                }
            }

            break;
        case CON_STATE_ERROR: /* transient */
            break;
        }

        if (done == -1) {
            done = 0;
        } else if (ostate == this->state) {
            std::cout << "read set" << std::endl; 
            done = 1;
        }
    }

    switch(state) {
    case CON_STATE_READ_POST:
    case CON_STATE_READ:
    case CON_STATE_CLOSE:
        ev->FdeventEventReset();
        ev->FdeventEventSet(this->fd, FDEVENT_IN);
        std::cout << "read fd"<< fd << std::endl; 
        break;
    case CON_STATE_WRITE:
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


