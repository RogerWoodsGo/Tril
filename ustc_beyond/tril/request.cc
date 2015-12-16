#include "request.h"
#include <string>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "utils.h"

namespace ustc_beyond {
namespace tril {

bool Request::HttpRequestParse() {
    std::string request_line;
    std::vector<std::string> vt;
    std::stringstream os(request_head);

    //parse request line
    std::getline(os, request_line);
    vt = StringSplit(trim(request_line), ' ');
    if(vt.size() == 3) {
        head_map["Method"] = vt[0];
        head_map["URI"] = vt[1];
        head_map["PROTOCOL"] = vt[2];
    }
    else
        return false;

    //parse request head
    std::string kv;
    while(std::getline(os, kv)){
        vt = StringSplit(trim(kv), ' ');
        head_map.insert(std::pair<std::string , std::string> (vt[0], vt[1]));
    }

    return true;
}

std::string Request::GetHeadValue(std::string key) {

    if(this->head_map.find(key) != this->head_map.end()) {
        return this->head_map[key];
    }
    else
        return std::string("");
}

}
}







