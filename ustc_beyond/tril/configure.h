#ifndef CONFIGURE_H_
#define CONFIGURE_H_
#include <getopt.h>
#include <unistd.h>
#include <strings.h>
#include<string>
#include <map>

namespace ustc_beyond {
namespace tril {

typedef struct opt{
    char* config_file;
    char* help;
    int daemonized;
} glob_option;

const option long_opts[] = {
    { "file", required_argument, NULL, 'f' },
    { "help", no_argument, NULL, 'h' },
    { "daemon", no_argument, NULL, 'D' },
    { NULL, no_argument, NULL, 0 }
};

class Configure {
public:
    bool GetOption(int argc, char* argv[]);
    bool LoadConfig();
    inline std::map<std::string, std::string> GetKeyValue(){
        return config_kv;
    }
    void Usage();
private:
    glob_option option_;
    std::map<std::string, std::string> config_kv;
};
}
}
#endif


