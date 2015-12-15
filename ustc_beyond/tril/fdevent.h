#ifndef FDEVENT_H_
#define FDEVENT_H_
#include <list>
#include "setting.h"
#include "iostrategy.h"
#include <iostream>

namespace ustc_beyond {
namespace tril {
//typedef handler_t (*fdevent_handler)(struct server *srv, void *ctx, int revents);
/* these are the POLL* values from <bits/poll.h> (linux poll)
 * */

#define FDEVENT_IN BV(0)
#define FDEVENT_PRI BV(1)
#define FDEVENT_OUT BV(2)
#define FDEVENT_ERR BV(3)
#define FDEVENT_HUP BV(4)
#define FDEVENT_NVAL BV(5)

typedef enum {
    FDEVENT_HANDLER_UNSET,
    FDEVENT_HANDLER_SELECT,
    FDEVENT_HANDLER_POLL,
    FDEVENT_HANDLER_LINUX_SYSEPOLL,
    FDEVENT_HANDLER_SOLARIS_DEVPOLL,
    FDEVENT_HANDLER_SOLARIS_PORT,
    FDEVENT_HANDLER_FREEBSD_KQUEUE,
    FDEVENT_HANDLER_LIBEV
} fdevent_handler_t;


class HandleFunc;

class Fdnode {
public:
    Fdnode(){};
    Fdnode(int fd, int revent, HandleFunc* hf, void* ctx) {
            this->fd = fd;
            this->revent = revent;
            this->hf = hf;
            this->ctx = ctx;
    };
    ~Fdnode(){};
    inline int GetFd() {
        return fd;
    };
    inline int GetRevent() {
        return revent;
    };
    inline void SetRevent(int revent) {
       this->revent = revent; 
    };
    inline void* GetCtx() {
        return ctx;
    };
    inline HandleFunc* GetHandleFunc() {
        return hf;
    };
private:
    int fd;
    int revent;
    void* ctx;
    HandleFunc* hf;
};


class Fdevent {
public:
    Fdevent(){};
    ~Fdevent(){};
    bool FdeventInit(int max_size, fdevent_handler_t type);
    bool FdnodeFree();
    void FdeventFree();
    bool FdeventRegister(int fd, HandleFunc* hf, void* ctx);
    bool FdeventUnregister(int fd);
    int  FdeventGetRevent(int index);
    HandleFunc*  FdeventGetHandleFunc(int fd);
    void*  FdeventGetContext(int index);
    bool FdeventEventSet(int fd, int revent);
    bool FdeventEventReset();
    bool FdeventEventUnset(int fd, int revent);
    bool FdeventEventDel(int fd);
    int  FdeventEventGetHappened(int index);
    int FdeventEventPoll(int timeout_ms);
    int FdeventFcntlSet(int fd);

private:
    Fdnode** fdarray;//nodes array
    IOStrategy* io_method;
    int max_size;
};

}
}

#endif



