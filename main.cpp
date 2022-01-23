// copyright 2021 miaosi@all rights reserved.
#include <pybind11/pybind11.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "board/god.h"
#include "board/board.h"


namespace py = pybind11;

PYBIND11_MODULE(cppjieqi, m) {
    py::class_<God>(m, "God")
        .def(py::init<>())
        .def("Initialize", &God::Initialize, py::arg("random") = true)
        .def("PrintPos", &God::PrintPos)
        .def("GetTurn", &God::GetTurn)
        .def("GetMeta", &God::GetMeta)
        .def("Move", py::overload_cast<const int, const int>(&God::Move))
        .def("Move", py::overload_cast<std::string>(&God::Move))
        .def("UndoMove", &God::UndoMove, py::arg("undo_times") = 1)
        .def("GenMoves", &God::GenMoves)
        .def("AIHint", &God::AIHint)
        .def("Single", &God::translate_single)
        .def("UCCI", &God::translate_ucci);
} 
