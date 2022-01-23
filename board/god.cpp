#include "god.h"

God::God(): MINGZI("RNBAKCP"){
    initialize_pst();
    initialize_L1();
    initialize_dir();
    board_pointer = new board::Board(pst, _dir);
    NEWRED; NEWBLACK;
}

God::~God(){
    if(board_pointer){
        delete board_pointer;
        board_pointer = NULL;
    }
    destroy_dir();
    destroy_L1();
    destroy_pst();
    if(ai) ai.reset();
}

void God::Initialize(bool random){
    if(random){
        board_pointer -> GenRandomBoard();
    }else{
        board_pointer -> Reset(NULL);
    }
}

bool God::PrintPos(){
    board_pointer -> PrintPos(board_pointer->turn, true, false, true);
    return board_pointer -> turn;
}

bool God::GetTurn(){
    return board_pointer -> turn;
}

py::dict God::GetMeta(){
    using namespace pybind11::literals;
    py::dict random_map_red, random_map_black, di1, di0;
    DIDICT(di1, board_pointer -> di_red);
    DIDICT(di0, board_pointer -> di_black);
    for(std::unordered_map<unsigned char, char>::iterator it = board_pointer -> random_map[true].begin(); it != board_pointer -> random_map[true].end(); ++it){
        random_map_red[py::int_(it -> first)] = std::string(1, it->second);
    }
    for(std::unordered_map<unsigned char, char>::iterator it = board_pointer -> random_map[false].begin(); it != board_pointer -> random_map[false].end(); ++it){
        random_map_black[py::int_(it -> first)] = std::string(1, it->second);
    }
    return py::dict("turn"_a = board_pointer -> turn, "state_red"_a = std::string(board_pointer -> state_red), \
        "state_black"_a = std::string(board_pointer -> state_black), "random_map_red"_a = random_map_red, "random_map_black"_a = random_map_black,\
        "di1"_a = di1, "di0"_a = di0);
}

inline std::shared_ptr<InfoDict> God::InnerMove(std::string s){
    return check_legal(s) ? board_pointer -> Move(s) : nullptr;
}

inline std::shared_ptr<InfoDict> God::InnerMove(const int encode_from, const int encode_to){
    return board_pointer -> Move(encode_from, encode_to);
}

py::dict God::_Move(std::shared_ptr<InfoDict> p){
    py::dict returndict, infodict, di1, di0;//di1: di_red, di0: di_black
    if(!p){
        returndict["di1"] = py::none();
        returndict["di0"] = py::none();
        returndict["infodict"] = py::none();
        return returndict;
    }
    InfoDict2pydict(infodict, p);
    returndict["infodict"] = infodict;
    DIDICT(di1, board_pointer -> di_red);
    returndict["di1"] = di1;
    DIDICT(di0, board_pointer -> di_black);
    returndict["di0"] = di0;
    return returndict;
}

py::dict God::Move(std::string s){
    return _Move(InnerMove(s));
}

py::dict God::Move(const int encode_from, const int encode_to){
    return _Move(InnerMove(encode_from, encode_to));
}

void God::UndoMove(int k){
    for(int i = 0; i < k; ++i){
        board_pointer -> UndoMove();
    }
}

py::list God::GenMoves(){
    py::list tmp = board_pointer -> GenMoves(true);
    return tmp;
}

py::tuple God::AIHint(){
    ai.reset(board_pointer->turn ? NEWRED : NEWBLACK);
    int src = 0, dst = 0;
    bool thinkres = ai -> Think(&src, &dst);
    return py::make_tuple(py::int_(src), py::int_(dst), py::bool_(thinkres));
}


