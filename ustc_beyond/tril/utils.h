#ifndef USTC_BEYOND_TRIL_UTILS_H_
#define USTC_BEYOND_TRIL_UTILS_H_

#include<stdint.h>
#include <string>
#include<vector>

namespace ustc_beyond {
namespace tril {

std::string GetCurrentFormatTime();

uint64_t GetCurrentMilisec();

std::vector<std::string> StringSplit(std::string src, char delim);
bool StringStartWith(std::string src, std::string prefix);
//bool StringStartWith(std::string src, const char* prefix);

}
}
#endif
