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
    bool ServerInit(int argc, char* argv[]);
    bool NetworkInit();
    bool WritePidfile();
    void ServerFree(){
        delete config;
    };
    void Daemonize();
    void Start();
    std::string GetConfigValue(const std::string& key);
    ~Server() {
        delete config;
    };
private:
    Server() {
        config = new Configure();
    };
    static void sigHandler(int sig_num);
    int MakeWorker(int worker); 

    Logging log;
    Configure* config;
    map<string, string> config_kv;
    static Server* instance_;
    static bool srv_shutdown;
    static bool graceful_shutdown;
};
}
}
#endif


