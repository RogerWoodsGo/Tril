#ifndef REQUEST_H_
#define REQUEST_H_
#include <map>
#include <string>

namespace ustc_beyond {
namespace tril {

class Request {
public:
    inline void RequestInit() {
        head_map["Method"] =  std::string("");
        head_map["URI"] =  std::string("");
        head_map["Protocol"] =  std::string("");
    };

    inline void RequestFree() {
    };

    inline void SetRequestHead(std::string head){
        request_head = head; 
    };

    inline void SetPostContent(std::string content){
        post_content = content;
    };

    inline bool HttpRequestIsPost(){
        if(GetHeadValue("Method") == "POST")
            return true;
        else
            return false;
    };

    std::string GetHeadValue(std::string key); 
    bool HttpRequestParse();

private:
    bool is_post;
    std::string request_head;
    std::string post_content;
    
    std::map<std::string, std::string> head_map;
};

}
}
#endif


