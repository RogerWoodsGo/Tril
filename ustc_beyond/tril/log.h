#ifndef USTC_BEYOND_TRIL_LOG_H_
#define USTC_BEYOND_TRIL_LOG_H_
#include <stdio.h>
#include <string>
#include <map>
namespace ustc_beyond {
namespace tril {
enum LogLevel {
    kInfo = 0,
    kDebug,
    kWarning,
    kError
};

class Logging {
public:
    Logging() {
        level_str[0] = "INFO";
        level_str[1] = "DEBUG";
        level_str[2] = "WARNING";
        level_str[3] = "ERROR";
    };
    inline void Config(const std::string& file_name, LogLevel level) {
        this->file_name = file_name;
        this->level = level;
        FILE* fp = (this->file_name == "") ? stdout:fopen(file_name.c_str(), "a+");
        this->fp = fp;
    };
    void Log(LogLevel level,  const char *fmt, ...);
    inline void Close() {
        fclose(this->fp);
    };
private:
    LogLevel level;
    FILE* fp;
    std::string file_name;
    std::string level_str[4];
};

}
}
#endif


