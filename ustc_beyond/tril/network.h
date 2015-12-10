#ifndef NETWORK_H_
#define NETWORK_H_
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <list>
#include "handlefunc.h"
#include "setting.h"

namespace ustc_beyond {
namespace tril {

typedef struct sockaddr sock_addr;
typedef struct sockaddr_in sock_addr_in;
typedef struct sockaddr_in6 sock_addr_in6;

class Connection;
class NetworkHandleFunc:public HandleFunc {
    handler_t FdeventHandler(Server* srv, void* ctx, int revents);
    Connection * ConnectionAccept(Server* srv, int fd);
};

class Network {
public:
    bool NetworkInit(Server* srv);
    bool NetworkRegisterFdevents(Server* srv);
    bool NetworkClose();
    int GetSockFd() {
        return sock_fd;
    };
private:
    HandleFunc* network_handle_func;
    std::list<Connection*> con;
    int sock_fd;
    int use_ipv6;


};

}
}
#endif


