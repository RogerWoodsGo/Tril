#ifndef RESPONSE_H_
#define RESPONSE_H_
#include<string>
#include<map>

namespace ustc_beyond {
namespace tril {
class Response {
public:

    void ResponseInit();
    void ResponseFree();
    bool GenerateFinalResponse();

    void SetHeadValue(std::string key, std::string value) {
        head_map[key] = value;
    };

    std::string GetHeadValue(const std::string& key) {
        if(this->head_map.find(key) != this->head_map.end()) {
            return this->head_map[key];
        }
        else
            return std::string("");

    };


    std::string GetReasonPhrase() {
        return status_reasonphrase[head_map["Status"]];
    };



    inline std::string GetFinalResponse() {
        return response;
    };

    inline void SetResponseEntity(const std::string& ety) {
        entity = ety;
    };

private:

    std::string entity;
    std::string response;
    std::map<std::string, std::string> status_reasonphrase;
    std::map<std::string, std::string> head_map;
};

}
}
#endif




