#ifndef thinker_h
#define thinker_h

#include "../global/global.h"

namespace board{
    struct Thinker{
        bool thinker_type; //true: Human, false: AI
        int retry_num;
        bool turn;
        virtual bool Think(int* src, int* dst) = 0;
    };
}

#endif
