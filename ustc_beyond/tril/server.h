#ifndef SERVER_H_
#define SERVER_H_

#include "configure.h"
#include "utils.h"
#include "log.h"
#include <map>

namespace ustc_beyond {
namespace tril {

class Network;
class Fdevent;
const int MAXFD  = 1024;
const int POLL_TIMEOUT = 10000000;

//Sigleton
class Server {
public:
    static Server* GetInstance() {
        if(instance_ == NULL) {
            return new Server();
        }
    };

    inline Configure* GetConfig(){
        return config;
    };
        
    inline Fdevent* GetFdevent(){
        return fdevent;
    };

    inline Network* GetNetwork(){
        return network;
    };

    ~Server() {
    };

    bool ServerInit(int argc, char* argv[]);
    bool ServerFree();
    bool WritePidfile();
    void Daemonize();
    void Start();
    //std::string GetConfigValue(const std::string& key);
    Logging log;

private:
    Server() {
    };
    static void sigHandler(int sig_num);
    int MakeWorker(int worker); 

    Configure* config;
    Fdevent* fdevent;
    Network* network;
    static Server* instance_;
    static bool srv_shutdown;
    static bool graceful_shutdown;
};

}
}
#endif


