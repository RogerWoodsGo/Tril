#ifndef FDEVENT_SELECT_H_
#define FDEVENT_SELECT_H_

#include <sys/select.h>
#include "setting.h"
#include "iostrategy.h"

namespace ustc_beyond {
namespace tril {
class Select:public IOStrategy {
public:
    bool EventInit();
    void EventFree();
    bool EventSet(int fd, int revent);
    bool EventReset();
    bool EventDel(int fd);
    int  EventGetHappened(int index);
    int EventPoll(int timeout_ms);

private:
    fd_set select_set_read;
    fd_set select_set_write;
    fd_set select_set_error;
    //Add three tmp fd to avoid reset, important 
    fd_set select_read; 
    fd_set select_write; 
    fd_set select_error; 

    int select_max_fd;
    int pre_max_fd;
};
}
}

#endif

