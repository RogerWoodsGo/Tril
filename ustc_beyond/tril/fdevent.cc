#include "fdevent.h"
#include "fdevent_epoll.h"
#include "fdevent_poll.h"
#include "fdevent_select.h"

namespace ustc_beyond {
namespace tril {
Fdnode* Fdevent::FdnodeInit(int fd, HandleFunc* hf, void* ctx) {
    Fdnode* node = new Fdnode(fd, 0, hf, ctx);
    return node;
}

bool Fdevent::FdnodeFree() {
    return true;
}

Fdevent* Fdevent::FdeventInit(int max_size, fdevent_handler_t type) {
    Fdevent* ev = new Fdevent();
    ev->fdarray = new Fdnode*[max_size];
    max_fd = -1;
    switch(type) {
    case FDEVENT_HANDLER_SELECT:
        ev->io_method = new Select();
        break;
    case FDEVENT_HANDLER_LINUX_SYSEPOLL:
        break;
    case FDEVENT_HANDLER_POLL:
        break;

    case FDEVENT_HANDLER_UNSET:
        break;
    default:
        break;
    }
    return ev;
}

bool Fdevent::FdeventFree() {
    return true;
}

bool Fdevent::FdeventRegister(int fd, HandleFunc* hf, void* ctx) {
    fdarray[fd] = FdnodeInit(fd, hf, ctx);
    return true;
}

bool Fdevent::FdeventUnregister(int fd) {
    delete fdarray[fd];
    fdarray[fd]  = NULL;
    return true;
}

int  Fdevent::FdeventGetRevent(int index) {
    return fdarray[index]->GetRevent();
}

inline HandleFunc*  Fdevent::FdeventGetHandleFunc(int fd) {
    return fdarray[fd]->GetHandleFunc();
}

inline void*  Fdevent::FdeventGetContext(int index) {
    return fdarray[index]->GetCtx();
}

bool Fdevent::FdeventEventSet(int fd, int revent) {
    if(fdarray[fd] == NULL)
        return false;
    if(!io_method->EventSet(fd, revent))
        return false;
    fdarray[fd]->SetRevent(revent);
    return true;

}

bool Fdevent::FdeventEventReset() {
    if(!io_method->EventReset())
        return false;
    return true;
}

bool Fdevent::FdeventEventDel(int fd) {
    if(fdarray[fd] == NULL)
        return false;
    if(!io_method->EventDel(fd))
        return false;
    fdarray[fd]->SetRevent(0);
    return true;
}

int  Fdevent::FdeventEventGetHappened(int index) {
    return io_method->EventGetHappened(index);
}

int Fdevent::FdeventEventPoll(int timeout_ms) {
    return io_method->EventPoll(timeout_ms);
}


}
}




