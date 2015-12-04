#include "log.h"
using namespace ustc_beyond::tril;
int main() {
//    ustc_beyond::tril::Func();
    Logging lg;
    lg.Config("log.inc", kDebug);
    lg.Log(kDebug, "string1");
    lg.Log(kInfo, "ok");
    lg.Log(kError, "ok");
    lg.Log(kError, " %s, %d", "this is", 4);
    lg.Close();
    return 0;
}

