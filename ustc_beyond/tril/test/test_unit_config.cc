#include "configure.h"
#include <iostream>
#include "log.h"

using namespace ustc_beyond::tril;

int main(int argc, char** argv){
    Configure* cfg = new Configure();
    cfg->GetOption(argc, argv);
    cfg->LoadConfig();
    std::map<std::string, std::string> kv = cfg->GetKeyValue();
    std::map<std::string, std::string>::iterator it;
    for(it = kv.begin(); it != kv.end(); it++){
        std::cout << (*it).first << " " << (*it).second << std::endl;
    }
    Logging log;
    log.Config(kv["logfile"], kDebug);
    log.Log(kError, "it is good or %s", "bad");
    
    delete cfg;
    return 0;
}
