#include "server.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include "utils.h"

namespace ustc_beyond {
namespace tril {
bool Server::ServerInit(int argc, char* argv[]) {
    //Config get option
    if(!config->GetOption(argc, argv)) {
        std::cerr << "Get option Error" << std::endl;
        return false;
    };

    if(!config->LoadConfig()) {
        std::cerr << "Load Configfile Error" << std::endl;
        return false;
    };
    config_kv = config->GetKeyValue();
    std::cout << "Here is ok" << std::endl;
    log.Config(GetConfigValue("logfile"), LogLevel(StringToNumber<int>(GetConfigValue("loglevel"))));
    if(!NetworkInit()) {
        log.Log(kError, "Network init error");
    }
    return true;
    //Daemonize();
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


std::string Server::GetConfigValue(const std::string& key) {
    if(this->config_kv.find(key) != this->config_kv.end()) {
        return this->config_kv[key];
    }
    else
        return string("");

}

void Server::sigHandler(int sig_num) {
    switch (sig_num) {
    case SIGTERM:
        Server::srv_shutdown = true;
        break;
    case SIGINT:
        //  if (Server::graceful_shutdown) Server::srv_shutdown = true;
        // else Server::graceful_shutdown = true;
        Server::srv_shutdown = true;
        break;
    case SIGALRM:
        //   handle_sig_alarm = true;
        break;
    //       case SIGHUP:  handle_sig_hup = 1; break;
    case SIGCHLD:
        break;
    }
}


bool Server::WritePidfile(){
    string pidfile = GetConfigValue("pidfile");

    fstream of;
    of.open(pidfile.c_str(), fstream::out);
    if(!of.is_open()){
        log.Log(kError, "Open pid file error");
        return false;
    }
    of << NumberToString<int>(getpid()) << std::endl;
    of.close();
    return true;
}


void Server::Start() {
    int worker = 0;
//signal catch
    struct sigaction act;
    act.sa_handler = Server::sigHandler;
    sigaction(SIGINT, &act, NULL);
    //daemonize
    if(config->IsDaemonize())
        Daemonize();
    //write pid file
    if(!WritePidfile()){
        log.Log(kError, "WritePidfile Error");
        return;
    }
    //fork worker
    string worker_str = GetConfigValue("worker");
    log.Log(kInfo, "worker is %s", worker_str.c_str());
    if(!(worker = StringToNumber<int>(worker_str))) {
        log.Log(kError, "There is no worker");
        return;
    }


    //do make worker
    int ret = MakeWorker(worker);
    if(-1 == ret) {
        log.Log(kError, "Make worker error");
        return;
    }
    else if(0 == ret) {
        log.Log(kInfo, "Exit");
        return;
    }

    //start outer loop
    while(!Server::srv_shutdown) {
        std::cout << "this is thread" << getpid() << std::endl;
        sleep(2);
    }
//    if(!this->is_freed)
//    ServerFree();
}

int Server::MakeWorker(int worker) {
    /* start watcher and workers */
    if (worker > 0) {
        int child = 0;
        //while (!child && !Server::srv_shutdown && !Server::graceful_shutdown) {
        while (!child && !Server::srv_shutdown) {
            if (worker > 0) {
                switch (fork()) {
                case -1:
                    return -1;
                case 0:
                    child = 1;
                    break;
                default:
                    worker--;
                    break;
                }
            } else {
                int status;

                log.Log(kInfo, "Start to wait worker %d", worker);
                if (-1 != wait(&status)) {
                    /**
                     *                   * one of our workers went away
                     *                                       */
                    log.Log(kError, "One worker went away %d", worker);
                    worker++;
                } else {
                    switch (errno) {
                    case EINTR:
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        if (!child) {
            if (Server::graceful_shutdown) {
                kill(0, SIGINT);
            } else if (Server::srv_shutdown) {
                kill(0, SIGTERM);
            }

            //log_error_close(srv);
            //network_close(srv);
            //connections_free(srv);
            //plugins_free(srv);
            ServerFree();
//            exit(0);
            return 0;
        }
    }
    return 1;
}

bool Server::NetworkInit() {
    return true;
}

Server* Server::instance_ = NULL;
bool Server::srv_shutdown = false;
bool Server::graceful_shutdown = false;

}
}




