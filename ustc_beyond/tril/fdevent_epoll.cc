#include "fdevent_epoll.h"
#include <string.h>
#include <unistd.h>
#include "fdevent.h"

namespace ustc_beyond {
namespace tril {

bool Epoll::EventSet(int fd, int revent) {
    struct epoll_event ep;
    memset(&ep, 0, sizeof(ep));
    int add = 0;
    ep.events = 0;

    if(fd_set.find(fd) == fd_set.end()){
        add = 1;
        fd_set.insert(fd);
    }

    if (revent & FDEVENT_IN)  ep.events |= EPOLLIN;
    if (revent & FDEVENT_OUT) ep.events |= EPOLLOUT;

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

bool Epoll::EventDel(int fd) {
	struct epoll_event ep;

	memset(&ep, 0, sizeof(ep));

	ep.data.fd = fd;
	ep.data.ptr = NULL;

	if (0 != epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ep)) {
		return false;
	}
    return true;
}

int  Epoll::EventGetHappened(int index) {
	size_t i;

    i = (index < 0) ? 0 : index + 1;

    return ep_events[i].data.fd;
}

int Epoll::EventPoll(int timeout_ms) {
    return epoll_wait(epoll_fd, ep_events, FDSIZE, timeout_ms);
}

bool Epoll::EventInit() {
    if(-1 == (epoll_fd = epoll_create(FDSIZE))) {
        return false;
    };
    return true;
}

void Epoll::EventFree() {
    close(epoll_fd);
}

}
}


