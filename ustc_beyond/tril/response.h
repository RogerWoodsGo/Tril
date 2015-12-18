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

    void SetHeadValue(std::string key, std::string value) {
        head_map[key] = value;
    };

    std::string GetReasonPhrase(const std::string& status){
        return status_reasonphrase[status]; 
    };


    std::string GenerateResponseString();

private:
    std::string response_content;
    std::map<std::string, std::string> status_reasonphrase;
    std::map<std::string, std::string> head_map;

};

}
}
#endif



