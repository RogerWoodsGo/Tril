#ifndef FDEVENT_EPOLL_H_ 
#define FDEVENT_EPOLL_H_
#endif
#include <sys/epoll.h>
#include <set>
#include "setting.h"
#include "iostrategy.h"

namespace ustc_beyond {
namespace tril {
const int FDSIZE = 1024;
class Epoll:public IOStrategy {
public:
    bool EventInit();
    void EventFree();
    bool EventSet(int fd, int revent);
    bool EventDel(int fd);
    int  EventGetHappened(int index);
    int EventPoll(int timeout_ms);

private:
    int epoll_fd;
    int happend_io;
    std::set<int> fd_set;
    struct epoll_event ep_events[FDSIZE]; 


    int select_max_fd;
};
}
}


