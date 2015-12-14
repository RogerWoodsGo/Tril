#ifndef IOSTRATEGY_H_
#define IOSTRATEGY_H_
#include "setting.h"
namespace ustc_beyond {
namespace tril {
class IOStrategy {
public:
    IOStrategy(){};
    virtual bool EventSet(int fd, int revent) = 0;
    virtual bool EventReset(){ return true;};
    virtual bool EventDel(int fd) = 0;
    virtual int  EventGetHappened(int index) = 0;
    virtual int EventPoll(int timeout_ms) = 0;
    virtual bool EventInit() = 0;
    virtual void EventFree(){};
    virtual ~IOStrategy(){};
};
}
}
#endif




