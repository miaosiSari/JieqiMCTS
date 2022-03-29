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
        .def("PrintPos", py::overload_cast<>(&God::PrintPos))
        .def("PrintPos", py::overload_cast<bool, bool, bool, bool>(&God::PrintPos))
        .def("GetTurn", &God::GetTurn)
        .def("GetState", &God::GetState)
        .def("GetMeta", &God::GetMeta)
        .def("Move", py::overload_cast<const int, const int, const bool>(&God::Move), py::arg("src"), py::arg("dst"), py::arg("check")=false)
        .def("Move", py::overload_cast<const std::string, const bool>(&God::Move), py::arg("ucci"), py::arg("check")=false)
        .def("UndoMove", &God::UndoMove, py::arg("undo_times") = 1)
        .def("GenMoves", &God::GenMoves)
        .def("AIHint", &God::AIHint, py::arg("depth") = 10, py::arg("di") = py::list())
        .def("Hint", &God::Hint, py::arg("depth") = 10, py::arg("di") = py::list())
        .def("Single", &God::translate_single)
        .def("UCCI", &God::translate_ucci)
        .def("Rough", &God::Rough, py::arg("discount_factor") = 1.5);
    py::class_<Node>(m, "Node")
        .def_readonly("turn", &Node::turn)
        .def_readonly("value", &Node::value)
        .def_readonly("n_visits", &Node::n_visits)
        .def_readonly("flag", &Node::flag)
        .def("size", &Node::size)
        .def("get", &Node::get)
        .def("pa", &Node::pa)
        .def("UCT", &Node::UCT)
        .def("expand", py::overload_cast<py::dict>(&Node::expand))
        .def("di", &Node::getdi)
        .def("meta", &Node::meta)
        .def("pos", &Node::pos)
        .def("state", &Node::getstate)
        .def("__str__", &Node::info);
    py::class_<Searcher>(m, "Searcher")
        .def(py::init<>())
        .def("Initialize", &Searcher::Initialize, py::arg("random") = true, \
            py::arg("turn") = true, py::arg("board") = "", py::arg("c_puct") = 5.0f, py::arg("prob") = 1.0f)
        .def("get", &Searcher::get, py::arg("return_root") = false)
        .def("UCT", py::overload_cast<>(&Searcher::UCT))
        .def("expand", py::overload_cast<py::dict>(&Searcher::expand))
        .def("child", &Searcher::child)
        .def("walk", &Searcher::walk)
        .def("reset", &Searcher::reset)
        .def("pos", &Searcher::pos, py::arg("print") = true)
        .def("ai", &Searcher::AIHint, py::arg("depth") = 8)
        .def("draw", &Searcher::draw)
        .def("copy", py::overload_cast<const int, const bool>(&Searcher::copy), py::arg("flag") = 3, py::arg("copy_root") = false)
        .def("move", py::overload_cast<const int, const int, const bool>(&Searcher::move), py::arg("src"), py::arg("dst"), py::arg("check")=true)
        .def("move", py::overload_cast<const std::string, const bool>(&Searcher::move), py::arg("ucci"), py::arg("check")=false)
        .def("flag", &Searcher::flag)
        .def("meta", &Searcher::meta)
        .def("state", &Searcher::getstate)
        .def("print", &Searcher::PrintPos)
        .def("rough", &Searcher::Rough, py::arg("discount_factor") = 1.5)
        .def("walkpa", &Searcher::walkpa)
        .def_readonly("god", &Searcher::g)
        .def_readwrite("c_puct", &Searcher::c_puct);
    m.def("UCCI", [](const int src, const int dst) -> std::string{
        int x1 = 12 - (src >> 4);
        int y1 = (src & 15) - 3;
        int x2 = 12 - (dst >> 4);
        int y2 = (dst & 15) - 3;
        std::string ret = "    ";
        ret[0] = 'a' + y1;
        ret[1] = '0' + x1;
        ret[2] = 'a' + y2;
        ret[3] = '0' + x2;
        return ret;
    });
    py::register_exception<Exception>(m, "Exception");
} 
