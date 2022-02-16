#ifndef thinker_h
#define thinker_h

#include "../global/global.h"
#include <pybind11/pybind11.h>
namespace py = pybind11;
namespace board{
    struct Thinker{
        bool thinker_type; //true: Human, false: AI
        int retry_num;
        bool turn;
        virtual bool Think(int maxdepth, unsigned char& src, unsigned char& dst, py::dict* valdict) = 0;
    };
}

#endif
