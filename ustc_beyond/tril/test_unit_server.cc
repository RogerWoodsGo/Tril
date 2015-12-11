#include "server.h"
#include<iostream>

using namespace ustc_beyond::tril;

int main(int argc, char* argv[]){
    Server* srv = Server::GetInstance();
    if(!srv->ServerInit(argc, argv)){
        std::cout << "Init server error" << std::endl;
        return -1;
    };
    srv->Start();
    delete srv;
    return 0;
}
