#include "fdevent_poll.h"

namespace ustc_beyond {
namespace tril {

bool Poll::EventInit() {
    return true;
}

bool Poll::EventSet(int fd, int revent) {
    return true;
}

bool Poll::EventReset() {
    return true;
}

bool Poll::EventDel(int fd) {
    return true;
}

int  Poll::EventGetHappened(int index) {
    return 0;
}

int Poll::EventPoll(int timeout_ms) {
    return 0;
}

}
}



