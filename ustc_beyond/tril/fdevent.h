#ifndef FDEVENT_H_
#define FDEVENT_H_
#include <list>

namespace ustc_beyond {
namespace tril {
    class Fdnode{
        public:
            virtual Handler
    };

    class Fdevent{
        public:
            Fdevent* CreateFdevent();
        private:
            std::list<Fdnode*> Fdevent;
    };
}
}

#endif
