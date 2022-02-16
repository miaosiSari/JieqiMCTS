// copyright 2021 miaosi@all rights reserved.
#include <pybind11/pybind11.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include "board/god.h"
#include "board/board.h"
#include "board/searcher.h"

namespace py = pybind11;

PYBIND11_MODULE(cppjieqi, m) {
    py::class_<God>(m, "God")
        .def(py::init<>())
        .def("Initialize", &God::Initialize, py::arg("random") = true, py::arg("turn") = true, py::arg("board") = "")
        .def("PrintPos", &God::PrintPos)
        .def("GetTurn", &God::GetTurn)
        .def("GetState", &God::GetState)
        .def("GetMeta", &God::GetMeta)
        .def("Move", py::overload_cast<const int, const int>(&God::Move))
        .def("Move", py::overload_cast<std::string>(&God::Move))
        .def("UndoMove", &God::UndoMove, py::arg("undo_times") = 1)
        .def("GenMoves", &God::GenMoves)
        .def("AIHint", &God::AIHint, py::arg("depth") = 10)
        .def("Hint", &God::Hint, py::arg("depth") = 10)
        .def("Single", &God::translate_single)
        .def("UCCI", &God::translate_ucci);
    py::class_<Node>(m, "Node")
        .def_readonly("turn", &Node::turn)
        .def_readonly("value", &Node::value)
        .def_readonly("n_visits", &Node::n_visits)
        .def("size", &Node::size)
        .def("get", &Node::get)
        .def("pa", &Node::pa)
        .def("UCT", &Node::UCT)
        .def("__str__", &Node::info);
    py::class_<Searcher>(m, "Searcher")
        .def(py::init<>())
        .def("Initialize", &Searcher::Initialize, py::arg("random") = true, \
            py::arg("turn") = true, py::arg("board") = "", py::arg("c_puct") = 5.0f, py::arg("prob") = 1.0f)
        .def("get", &Searcher::get, py::arg("return_root") = false)
        .def("UCT", py::overload_cast<>(&Searcher::UCT))
        .def("expand", py::overload_cast<py::dict>(&Searcher::expand))
        .def_readonly("god", &Searcher::g)
        .def_readwrite("c_puct", &Searcher::c_puct);
    py::register_exception<Exception>(m, "Exception");
} 
