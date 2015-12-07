#include "server.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <iostream>

namespace ustc_beyond {
namespace tril {
void Server::ServerInit(int argc, char* argv[]) {
    //Config get option
    if(!config->GetOption(argc, argv)) {
        std::cerr << "Get option Error" << std::endl;
        return;
    };

    if(!config->LoadConfig()) {
        std::cerr << "Load Configfile Error" << std::endl;
        return;
    };
    config_kv = config->GetKeyValue();
    if(!NetworkInit()){
        log.Log(kError, "Network init error");
    }
    Daemonize();
}

void Server::Daemonize() {
#ifdef SIGTTOU
    signal(SIGTTOU, SIG_IGN);
#endif
#ifdef SIGTTIN
    signal(SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTSTP
    signal(SIGTSTP, SIG_IGN);
#endif
    if (0 != fork()) exit(0);

    if (-1 == setsid()) exit(0);

    signal(SIGHUP, SIG_IGN);

    if (0 != fork()) exit(0);

    if (0 != chdir("/")) exit(0);

}


void Server::Start() {

}

bool Server::NetworkInit() {
return true;
}

Server* Server::instance_ = NULL;

}
}






