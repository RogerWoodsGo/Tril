#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <sys/socket.h>
#include <sys/types.h>

namespace ustc_beyond {
namespace tril {

    class Network;
class Connection {
    public: 
        Connection *connection_accept(Network* net, int fd);

};
}
}
#endif

