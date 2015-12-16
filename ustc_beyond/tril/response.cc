#include "response.h"
namespace ustc_beyond {
namespace tril {

void Response::ResponseInit() {
    head_map["Protocol"] = std::string("");
    head_map["Status"] = std::string("");
    head_map["Transfer-Encoding"] = std::string("chunked");
//        head_map["ReasonPhrase"] = "";
    status_reasonphrase["200"] = std::string("OK");
    status_reasonphrase["404"] = std::string("Not Found");
}


void Response::ResponseFree() {
}

}
}


