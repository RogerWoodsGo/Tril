#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <deque>
#include <string>

#include "setting.h"
#include "handlefunc.h"

namespace ustc_beyond {
namespace tril {

class Network;
class Request;
class Response;
class Configure;

const int READ_BUFFER_SIZE = 1024;
class Connection {
public:
    //Connection *connection_accept(Network* net, int fd);
    bool ConnectionReset();
    bool ConnectionReadFromFd();
    int ConnectionParseRequest();
    int ConnectionGenerateResponse();
    bool ConnectionWriteToFd();
    bool ConnectionClose();
    bool ConnectionStateMachine(Server* srv);

    Connection(Configure* conf){
       this->config = conf; 
    };

    inline void ConnectionSetFd(int fd) {
        this->fd = fd;
    };

    inline int ConnectionGetFd() {
        return this->fd;
    };

    inline void ConnectionSetReadable(bool readable) {
        this->is_readable =  readable;
    };

    inline bool ConnectionGetReadable() {
        return this->is_readable;
    };

    inline void ConnectionSetWriteable(bool writable) {
        this->is_writable =  writable;
    };

    inline bool ConnectionGetWriteable() {
        return this->is_writable;
    }

    inline void ConnectionSetKeepalived(bool keeplived) {
        this->is_keepalived =  keeplived;
    };

    inline bool ConnectionGetKeepalived() {
        return this->is_keepalived;
    }

    inline void ConnectionSetState(connection_state_t sta) {
        this->state = sta;
    };

    inline connection_state_t ConnectionGetState() {
        return state;
    };

    inline void ConnectionSetClientIp(const std::string& ip) {
        this->cli_addr = ip;
    };

    inline std::string ConnectionGetClientIp() {
        return this->cli_addr;
    };

    inline bool ConnectionWriteQueueEmpty() {
        return this->write_queue.empty();
    };

    inline bool ConnectionReadQueueEmpty() {
        return this->read_queue.empty();
    };

private:
    std::string ConnectionMergeQueue(std::deque<string> queue);
    void ConnectionAdjustQueue(std::deque<std::string> queue, std::string newdata);

    bool is_keepalived;
    bool is_readable;
    bool is_writable;
    int fd;//socket fd
    std::deque<std::string> read_queue;
    std::deque<std::string> write_queue;
    connection_state_t state;
    std::string cli_addr;
    Request* request;
    Response* response;
    Configure* config;
//    typedef std::deque<std::string>::iterator iter; 
};

}
}
#endif



