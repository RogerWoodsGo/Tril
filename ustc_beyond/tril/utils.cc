#include "utils.h"
#include<unistd.h>
#include<sys/time.h>
#include <sys/stat.h>
#include<stdio.h>
#include<time.h>
#include<iostream>
#include<sstream>
#include<algorithm>
#include <functional> 
#include <cctype>
#include <locale>


namespace ustc_beyond {
namespace tril {
using namespace std;

std::string GetCurrentFormatTime() {
    struct timeval tv;
    char buf[64];

    gettimeofday(&tv,NULL);
    size_t off = strftime(buf,sizeof(buf),"%d %b %H:%M:%S.",localtime(&tv.tv_sec));
    snprintf(buf+off,sizeof(buf)-off,"%03d",(int)tv.tv_usec/1000);
    return std::string(buf);

}

uint64_t GetCurrentMilisec() {
    time_t sec;
    uint64_t msec;
    uint64_t current_msec_;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    sec = tv.tv_sec;
    msec = tv.tv_usec / 1000;

    current_msec_ = sec * 1000 + msec;
    return current_msec_;
}

vector<string> StringSplit(string src, char delim) {
    vector<string> res;
    std::stringstream os(src);
    string tmp_str;

    while(std::getline(os, tmp_str, delim)) {
        res.push_back(tmp_str);
    }

    return res;
}

bool StringStartWith(const string& src, const string& prefix) {
    if(src.size() == 0)
        return false;
    size_t pos = src.find(prefix);
    if(pos == 0)
        return true;
    else
        return false;
}

bool StringEndWith(const string& src, const string& suffix) {
    if(src.size() == 0)
        return false;
    size_t pos = src.rfind(suffix);
    if((pos + suffix.size()) == src.size())
        return true;
    else
        return false;
}

// trim from start
std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

bool FileExist(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

}
}



