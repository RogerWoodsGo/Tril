#include "server.h"

using namespace ustc_beyond::tril;

int main(int argc, char* argv[]){
    Server* srv = Server::GetInstance();
    srv->ServerInit(argc, argv);
    delete srv;
    return 0;
}
