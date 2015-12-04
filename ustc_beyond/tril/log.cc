#include "log.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include "utils.h"

namespace ustc_beyond {
namespace tril {

void Logging::Log(LogLevel level,  const char *fmt, ...) {
    if(level >= this->level) {
        char* msg;
        va_list ap;
        va_start(ap, fmt);
        vasprintf(&msg, fmt, ap);
        va_end(ap);

        fprintf(fp,"%s %d [%s]:%s\n", GetCurrentFormatTime().c_str(), getpid(), level_str[level].c_str(), msg);
        fflush(fp);
        free(msg);
    }
}

}
}



