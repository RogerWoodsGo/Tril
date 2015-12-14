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
class Network;

class ConnectionHandleFdevent:public HandleFunc{
    handler_t FdeventHandler(Server* srv, void* ctx, int revents);
};

class NetworkHandleFunc:public HandleFunc {
    handler_t FdeventHandler(Server* srv, void* ctx, int revents);
    Connection * ConnectionAccept(Server* srv);
};

class Network {
public:
    bool NetworkInit(Server* srv);
    bool NetworkRegisterFdevents(Server* srv);
    Connection* GetNewConnection();
    bool NetworkClose();

    inline int GetSockFd() {
        return sock_fd;
    };

    inline HandleFunc* GetHandleFunc(){
        return connect_handle_func;
    };
private:
    HandleFunc* network_handle_func;
    HandleFunc* connect_handle_func;
    std::list<Connection*> con;
    int sock_fd;
    int use_ipv6;


};

}
}
#endif


