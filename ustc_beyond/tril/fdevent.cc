#include "fdevent.h"
#include "fdevent_epoll.h"
#include "fdevent_poll.h"
#include "fdevent_select.h"
#include "handlefunc.h"
#include <iostream>

namespace ustc_beyond {
namespace tril {

bool Fdevent::FdnodeFree() {
    return true;
}

bool Fdevent::FdeventInit(int max_size, fdevent_handler_t type) {
    fdarray = new Fdnode* [max_size];
    if(fdarray == NULL){
        return false;
    }
    this->max_size = max_size;
    switch(type) {
    case FDEVENT_HANDLER_SELECT:
        io_method = new Select();
        break;
    case FDEVENT_HANDLER_LINUX_SYSEPOLL:
        io_method = new Epoll();
        break;
    case FDEVENT_HANDLER_POLL:
        break;

    case FDEVENT_HANDLER_UNSET:
        break;
    default:
        break;
    }
    return true;
}

void Fdevent::FdeventFree() {
    delete []fdarray; 
    delete io_method;
}

bool Fdevent::FdeventRegister(int fd, HandleFunc* hf, void* ctx) {
    fdarray[fd] = new Fdnode(fd, FDEVENT_NVAL, hf, ctx);
    if(fdarray[fd] == NULL){
        return false;
    }
    
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

HandleFunc*  Fdevent::FdeventGetHandleFunc(int fd) {
    return fdarray[fd]->GetHandleFunc();
}

void*  Fdevent::FdeventGetContext(int index) {
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




