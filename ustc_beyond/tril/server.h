#ifndef SERVER_H_
#define SERVER_H_
#include "server.h"

#include "configure.h"
#include "utils.h"
#include "log.h"
#include <map>

namespace ustc_beyond {
namespace tril {
//Sigleton
class Server {
public:
    static Server* GetInstance() {
        if(instance_ == NULL) {
            return new Server();
        }
    };
    void ServerInit(int argc, char* argv[]);
    void Daemonize();
    void Start();
    bool NetworkInit();
    ~Server() {
        delete config;
    };
private:
    Logging log;
    Configure* config;
    Server() {
        config = new Configure();
    };
    static Server* instance_;
    map<string, string> config_kv;
};
}
}
#endif


