#include "fdevent_epoll.h"
#include <string.h>
#include "fdevent.h"

namespace ustc_beyond {
namespace tril {

bool Epoll::EventSet(int fd, int revent) {
    struct epoll_event ep;
    memset(&ep, 0, sizeof(ep));
    int add = 0;

    ep.events = 0;

    if (revent & FDEVENT_IN)  ep.events |= EPOLLIN;
    if (revent & FDEVENT_OUT) ep.events |= EPOLLOUT;
    if (revent & FDEVENT_NVAL) add = 1;

    /**
     *
     * with EPOLLET we don't get a FDEVENT_HUP
     * if the close is delay after everything has
     * sent.
     *
     */

    ep.events |= EPOLLERR | EPOLLHUP /* | EPOLLET */;

    ep.data.ptr = NULL;
    ep.data.fd = fd;

    if (0 != epoll_ctl(epoll_fd, add ? EPOLL_CTL_ADD : EPOLL_CTL_MOD, fd, &ep)) {

        return 0;
    }
    return fd;
}

bool Epoll::EventReset() {
    return true;
}

bool Epoll::EventDel(int fd) {
    return true;
}
int  Epoll::EventGetHappened(int index) {
    return 0;
}
int Epoll::EventPoll(int timeout_ms) {
    return 0;
}
bool Epoll::EventInit() {
    if(-1 == (epoll_fd = epoll_create(1024))) {
        return false;
    };
    return true;
}
}
}


