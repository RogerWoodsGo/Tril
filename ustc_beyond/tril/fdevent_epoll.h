#ifndef FDEVENT_EPOLL_H_ 
#define FDEVENT_EPOLL_H_
#endif
#include <sys/epoll.h>
#include "setting.h"
#include "iostrategy.h"

namespace ustc_beyond {
namespace tril {
class Epoll:public IOStrategy {
public:
    bool EventInit();
    bool EventSet(int fd, int revent);
    bool EventReset();
    bool EventDel(int fd);
    int  EventGetHappened(int index);
    int EventPoll(int timeout_ms);

private:
    int epoll_fd;
    struct epoll_event ep_events[1000]; 

    int select_max_fd;
};
}
}


