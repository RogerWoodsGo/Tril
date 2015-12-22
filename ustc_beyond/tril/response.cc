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


bool Response::GenerateFinalResponse() {
    //status line
    response += "HTTP/1.1 ";
    response += head_map["Status"];
    response += " ";
    response += GetReasonPhrase();
    response += "\r\n";
    //response head
    if(head_map["Server"] != "") {
        response += "Server:";
        response +=   head_map["Server"];
        response += "\r\n";

    }

    if(head_map["Date"] != "") {
        response += "Date:";
        response +=   head_map["Date"];
        response += "\r\n";
    }

    //entity head
    if(head_map["Content-Length"] != "") {
        response += "Content-Length:";
        response +=   head_map["Content-Length"];
        response += "\r\n";
    }
    //entity
    response += "\r\n";
    response += entity;
    return true;
}

void Response::ResponseFree() {
}

}
}





