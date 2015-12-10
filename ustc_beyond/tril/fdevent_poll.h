#ifndef FDEVENT_POLL_H_ 
#define FDEVENT_POLL_H_
#endif
#include <sys/poll.h>
#include "setting.h"
#include "iostrategy.h"

namespace ustc_beyond {
namespace tril {
class Poll:public IOStrategy {
public:
    bool EventInit();
    void EventFree();
    bool EventSet(int fd, int revent);
    bool EventReset();
    bool EventDel(int fd);
    int  EventGetHappened(int index);
    int EventPoll(int timeout_ms);

private:
    int select_max_fd;
};

}
}


