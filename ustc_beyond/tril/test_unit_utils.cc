#include <iostream>
#include "utils.h"

using namespace std;
using ::ustc_beyond::tril::StringStartWith;

int main(){
    std::vector<std::string> vt = ::ustc_beyond::tril::StringSplit("hello world ni hao", ',');
    cout << StringStartWith(std::string("abcde"), string("abcdef")) << endl;
    cout << StringStartWith(std::string("abcde"), "abc") << endl;
    //cout << StringStartWith(std::string("abcde"), "") << endl;
    for(int x = 0; x < vt.size(); x++){
        std::cout << vt[x] << std::endl;
    }
    return 0;
}
