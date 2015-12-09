#include "fdevent_select.h"
#include "fdevent.h"

namespace ustc_beyond {
namespace tril {

bool Select::EventInit() {
    return true;
}

bool Select::EventSet(int fd, int revent) {

    if (revent & FDEVENT_IN) {
        FD_SET(fd, &(select_set_read));
    } else {
        FD_CLR(fd, &(select_set_read));
    }
    if (revent & FDEVENT_OUT) {
        FD_SET(fd, &(select_set_write));
    } else {
        FD_CLR(fd, &(select_set_write));
    }
    FD_SET(fd, &(select_set_error));

    if (fd > select_max_fd) select_max_fd = fd;

    return true;
}

bool Select::EventReset() {

    FD_ZERO(&(select_set_read));
    FD_ZERO(&(select_set_write));
    FD_ZERO(&(select_set_error));
    select_max_fd = -1;
    return true;
}

bool Select::EventDel(int fd) {

    FD_ZERO(&(select_set_read));
    FD_ZERO(&(select_set_write));
    FD_ZERO(&(select_set_error));
    select_max_fd = -1;

    return true;
}

int  Select::EventGetHappened(int index) {
    int i;
    i = (index < 0) ? 0 : index + 1;

    for (; i < select_max_fd + 1; i++) {
        if (FD_ISSET(i, &(select_read))) return i;
        if (FD_ISSET(i, &(select_write))) return i;
        if (FD_ISSET(i, &(select_error))) return i;
    }

    return -1;
}

int Select::EventPoll(int timeout_ms) {
    struct timeval tv;

    tv.tv_sec =  timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    select_read = select_set_read;
    select_write = select_set_write;
    select_error = select_set_error;

    return select(select_max_fd + 1, &(select_read), &(select_write), &(select_error), &tv);
}

}
}



