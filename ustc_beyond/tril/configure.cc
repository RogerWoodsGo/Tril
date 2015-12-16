#include "configure.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "utils.h"

namespace ustc_beyond {
namespace tril {

void Configure::Usage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "-f|--file <file_name> You should provide the configure file" << std::endl;
    std::cout << "-D|--Daemon Given 0 or 1 to decide whether to daemonize" << std::endl;
    std::cout << "-h|-?|--help Show the help information" << std::endl;
}

std::string Configure::GetConfigValue(const std::string& key) {
    if(this->config_kv.find(key) != this->config_kv.end()) {
        return this->config_kv[key];
    }
    else
        return std::string("");

}

bool Configure::GetOption(int argc, char* argv[]) {
    int opt = 0, options_index = 0;

    if(argc == 1) {
        Usage();
        return false;
    }

    while((opt = getopt_long(argc, argv, "?Df:h", long_opts, &options_index)) != -1)
    {
        switch(opt)
        {
        case  0 :
            break;
        case 'f':
            option_.config_file = optarg;
            break;
        case 'D':
            option_.daemonized = 1;
            break;
        case 'h':
        case '?':
            Usage();
            break;
        default:
            break;
        }
    }
    if(option_.config_file == NULL) {
        Usage();
        return false;
    }

    return true;
}



void Configure::SetDefault() {
    config_kv["logfile"] = "tril.log";
    config_kv["level"] = "0";
    config_kv["worker"] = "3";
    config_kv["debug"] = "1";
    config_kv["port"] = "80";
    config_kv["hostname"] = "localhost";
    config_kv["ipaddr"] = "127.0.0.1";
    config_kv["pidfile"] = "tril.pid";
    config_kv["htdoc"] = "htdoc";
}

bool Configure::LoadConfig() {
    fstream fp;
    fp.open(option_.config_file, fstream::in);
    std::string line;
    std::vector<std::string> vt;

    
    if(!fp.is_open()) {
        std::cerr <<"cann't open configure file" << std::endl;
    }

    while(getline(fp, line)) {
        std::string trim_line = trim(line);
        if(StringStartWith(trim_line, std::string("#")) || trim_line == "")
            continue;
        else {
            vt = StringSplit(trim_line, '=');
            if(vt.size() < 2) {
                std::cerr << "cann't open configure file" << std::endl;
                return false;
                break;
            }
            if(trim(vt[0]) == "logfile")
                config_kv["logfile"] = trim(vt[1]);
            else if(trim(vt[0]) == "port")
                config_kv["port"] = trim(vt[1]);
            else if(trim(vt[0]) == "hostname")
                config_kv["hostname"] = trim(vt[1]);
            else if(trim(vt[0]) == "ipaddr")
                config_kv["ipaddr"] = trim(vt[1]);
            else if(trim(vt[0]) == "htroot")
                config_kv["htroot"] = trim(vt[1]);
            else if(trim(vt[0]) == "debug")
                config_kv["debug"] = trim(vt[1]);
            else if(trim(vt[0]) == "pidfile")
                config_kv["pidfile"] = trim(vt[1]);
            else if(trim(vt[0]) == "worker")
                config_kv["worker"] = trim(vt[1]);
            else if(trim(vt[0]) == "loglevel")
                config_kv["loglevel"] = trim(vt[1]);

        }
    }//end while
    fp.close();
    return true;
}


}
}


