#include "server.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include "utils.h"
#include "fdevent.h"
#include "network.h"
#include "handlefunc.h"

namespace ustc_beyond {
namespace tril {

bool Server::ServerInit(int argc, char* argv[]) {
    config = new Configure();
    fdevent = new Fdevent();
    network = new Network();
    //Config get option
    if(!config->GetOption(argc, argv)) {
        std::cerr << "Get option Error" << std::endl;
        return false;
    };

    if(!config->LoadConfig()) {
        std::cerr << "Load Configfile Error" << std::endl;
        return false;
    };
    std::cout << "Here is ok" << std::endl;
    log.Config(config->GetConfigValue("logfile"), LogLevel(StringToNumber<int>(config->GetConfigValue("loglevel"))));

    if(!network->NetworkInit(this)) {
        log.Log(kError, "Network init error");
        return false;
    }
    return true;
    //Daemonize();
}

bool Server::ServerFree() {
    delete config;
    delete fdevent;
    delete network;
    return true;
};

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


bool Server::WritePidfile() {
    string pidfile = config->GetConfigValue("pidfile");

    fstream of;
    of.open(pidfile.c_str(), fstream::out);
    if(!of.is_open()) {
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
    if(!WritePidfile()) {
        log.Log(kError, "WritePidfile Error");
        return;
    }
    //fork worker
    string worker_str = config->GetConfigValue("worker");
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

    if(!fdevent->FdeventInit(MAXFD, FDEVENT_HANDLER_SELECT)) {
        log.Log(kError, "Event init error");
        fdevent->FdeventFree();
        //ServerFree();
        return;
    }

    if(!network->NetworkRegisterFdevents(this)) {
        log.Log(kError, "Event init error");
        //ServerFree();
        return;
    }

    //start outer loop
    int n = -1;
    while(!Server::srv_shutdown) {
        if((n = fdevent->FdeventEventPoll(POLL_TIMEOUT)) > 0 ) {
            int fd_ndx = -1;
            int revents;

            std::cout << "this is thread " << getpid() << "total event" << n << std::endl;
            do {
                HandleFunc* handler;
                void *context;
                handler_t r;

                fd_ndx  = fdevent->FdeventEventGetHappened(fd_ndx);
                if (-1 == fd_ndx) break;

                revents = fdevent->FdeventGetRevent(fd_ndx);
                handler = fdevent->FdeventGetHandleFunc(fd_ndx);
                context = fdevent->FdeventGetContext(fd_ndx);
                r = handler->FdeventHandler(this, context, revents);

                switch(r) {
                case HANDLER_FINISHED:
                case HANDLER_GO_ON:
                    log.Log(kInfo, "Event handle go on");
                    break;
                default:
                    log.Log(kInfo, "Event handle error");
                    break;
                }
            } while(--n > 0);

        }
        sleep(2);
    }
//    if(!this->is_freed)
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


Server* Server::instance_ = NULL;
bool Server::srv_shutdown = false;
bool Server::graceful_shutdown = false;

}
}








