#ifndef HANDLEFUNC_H_
#define HANDLEFUNC_H_

#include "setting.h"
#include "server.h"

namespace ustc_beyond {
namespace tril {

class HandleFunc {
public:
    HandleFunc(){};
    virtual handler_t FdeventHandler(Server* srv, void* ctx, int revents) = 0;
    virtual ~HandleFunc(){};
};

}
}
#endif




