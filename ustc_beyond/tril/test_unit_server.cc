#include "server.h"

using namespace ustc_beyond::tril;

int main(int argc, char* argv[]){
    Server* srv = Server::GetInstance();
    if(!srv->ServerInit(argc, argv)){
        return -1;
    };
    srv->Start();
    delete srv;
    return 0;
}
