// copyright 2021 miaosi@all rights reserved.
//#include <pybind11/pybind11.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "global/global.h"
#include "board/god.h"
#include "board/board.h"

/*
namespace py = pybind11;

int k = 2;
int add(int i, int j) {
    return i + j + k;
}

PYBIND11_MODULE(cppjieqi, m) {
    m.doc() = "pybind11 example plugin";
    m.def("add", &add, "A function which adds two numbers");
} 
*/

extern God* g;

int main(void) {
    srand(time(NULL));
    g = new God("../players.conf");
    DEBUG ? g -> StartGame() : g -> StartGameLoopAlternatively();
    delete g;
    return 0;
}