#ifndef USTC_BEYOND_TRIL_UTILS_H_
#define USTC_BEYOND_TRIL_UTILS_H_

#include<stdint.h>
#include <string>
#include <sstream>
#include<vector>
using namespace std;

namespace ustc_beyond {
namespace tril {

std::string GetCurrentFormatTime();

uint64_t GetCurrentMilisec();

std::vector<std::string> StringSplit(std::string src, char delim);
bool StringStartWith(const std::string& src, const std::string& prefix);
bool StringEndWith(const std::string& src, const std::string& suffix);
//bool StringStartWith(std::string src, const char* prefix);
std::string &ltrim(std::string &s);
std::string &rtrim(std::string &s);
std::string &trim(std::string &s);

template <typename T> std::string NumberToString ( T Number ){
    std::stringstream ss;
    ss << Number;
    return ss.str();
};

template <typename T> T StringToNumber ( const string &Text ){                    
    stringstream ss(Text);
    T result;
    return ss >> result ? result : 0;
};

bool FileExist(const std::string& name);

}
}
#endif

