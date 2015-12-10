#ifndef SERVER_H_
#define SERVER_H_

#include "configure.h"
#include "utils.h"
#include "log.h"
#include <map>

namespace ustc_beyond {
namespace tril {
//Sigleton
class Network;
class Fdevent;
class Server {
public:
    static Server* GetInstance() {
        if(instance_ == NULL) {
            return new Server();
        }
    };

    void ServerFree(){
        delete config;
    };
    
    inline Fdevent* GetFdevent(){
        return fdevent;
    };

    inline Network* GetNetwork(){
        return network;
    };

    ~Server() {
        delete config;
    };

    bool ServerInit(int argc, char* argv[]);
    bool NetworkInit();
    bool WritePidfile();
    void Daemonize();
    void Start();
    std::string GetConfigValue(const std::string& key);
    Logging log;
private:
    Server() {
        config = new Configure();
    };
    static void sigHandler(int sig_num);
    int MakeWorker(int worker); 

    Configure* config;
    Fdevent* fdevent;
    Network* network;
    map<string, string> config_kv;
    static Server* instance_;
    static bool srv_shutdown;
    static bool graceful_shutdown;
};

}
}
#endif


