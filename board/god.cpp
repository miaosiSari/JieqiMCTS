#include "god.h"

God::God(): MINGZI("RNBAKCP"){
    tptable = new (std::nothrow) tp[MAX_ZOBRIST];
    initialize_pst();
    initialize_L1();
    initialize_dir();
    board_pointer = new board::Board(pst, _dir);
}

God::~God(){
    if(tptable){
        delete [] tptable;
        tptable = NULL;
    }
    if(board_pointer){
        delete board_pointer;
        board_pointer = NULL;
    }
    destroy_dir();
    destroy_L1();
    destroy_pst();
    if(ai) ai.reset();
}

std::string God::__Adapter(std::string& board){
    std::string emptyline(16, ' '), prefix(3, ' '), appendix(4, ' ');
    std::string result = (emptyline +  emptyline + emptyline);
    for(int i = 0; i < 10; ++i){
        result += (prefix + board.substr(9*i, 9) + appendix);
    }
    result += (emptyline +  emptyline + emptyline);
    return result;
}

bool God::Initialize(bool random, bool turn, std::string board){
    std::string result;
    bool need = false;
    if(!random && board.size() == 90){
        result = __Adapter(board);
        need = true;
    }
    if(random){
        board_pointer -> GenRandomBoard(turn);
    }else{
        board_pointer -> Reset(turn, need ? &result : nullptr);
    }
    return need;
}

bool God::PrintPos(){
    board_pointer -> PrintPos(board_pointer->turn, true, false, true);
    return board_pointer -> turn;
}

bool God::GetTurn(){
    return board_pointer -> turn;
}

py::tuple God::GetState(){
    return py::make_tuple(py::bool_(board_pointer -> finished), py::int_(board_pointer -> state));
}

py::dict God::GetMeta(){
    using namespace pybind11::literals;
    py::dict random_map_red, random_map_black;
    py::list di1, di0;
    DILIST(di1, board_pointer -> di_red);
    DILIST(di0, board_pointer -> di_black);
    for(std::unordered_map<unsigned char, char>::iterator it = board_pointer -> random_map[true].begin(); it != board_pointer -> random_map[true].end(); ++it){
        random_map_red[py::int_(it -> first)] = std::string(1, it->second);
    }
    for(std::unordered_map<unsigned char, char>::iterator it = board_pointer -> random_map[false].begin(); it != board_pointer -> random_map[false].end(); ++it){
        random_map_black[py::int_(it -> first)] = std::string(1, it->second);
    }
    return py::dict("turn"_a = board_pointer -> turn, "state_red"_a = std::string(board_pointer -> state_red), \
        "state_black"_a = std::string(board_pointer -> state_black), "random_map_red"_a = random_map_red, "random_map_black"_a = random_map_black,\
        "di1"_a = di1, "di0"_a = di0, "state"_a = GetState());
}

inline std::shared_ptr<InfoDict> God::InnerMove(const std::string s, bool check){
    return check_legal(s) ? board_pointer -> Move(s, check) : nullptr;
}

inline std::shared_ptr<InfoDict> God::InnerMove(const int encode_from, const int encode_to, bool check){
    return board_pointer -> Move(encode_from, encode_to, check);
}

py::dict God::_Move(std::shared_ptr<InfoDict> p){
    py::dict returndict, infodict;
    py::list di1, di0;//di1: di_red, di0: di_black
    returndict["state"] = GetState();
    if(!p){
        returndict["infodict"] = py::none();
        returndict["di1"] = py::none();
        returndict["di0"] = py::none();
        return returndict;
    }
    InfoDict2pydict(infodict, p);
    returndict["infodict"] = infodict;
    DILIST(di1, board_pointer -> di_red);
    returndict["di1"] = di1;
    DILIST(di0, board_pointer -> di_black);
    returndict["di0"] = di0;
    return returndict;
}

py::dict God::Move(const std::string s, bool check){
    if(check){
        board_pointer -> GenMoves(true);
    }
    return _Move(InnerMove(s, check));
}

py::dict God::Move(const int encode_from, const int encode_to, bool check){
    if(check){
        board_pointer -> GenMoves(true);
    }
    return _Move(InnerMove(encode_from, encode_to, check));
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

bool God::__InnerHint(int depth, unsigned char& src, unsigned char& dst, py::dict* valdict, py::list& di){
    unsigned char ditmp[2][123];
    bool divalid = true;
    size_t counter = 0;
    for(py::handle obj: di){
        unsigned char tmp = obj.cast<unsigned char>();
        switch(counter){
            case 0: ditmp[1][(int)'R'] = tmp; break;
            case 1: ditmp[1][(int)'N'] = tmp; break;
            case 2: ditmp[1][(int)'B'] = tmp; break;
            case 3: ditmp[1][(int)'A'] = tmp; break;
            case 4: ditmp[1][(int)'C'] = tmp; break;
            case 5: ditmp[1][(int)'P'] = tmp; break;
            case 6: ditmp[0][(int)'r'] = tmp; break;
            case 7: ditmp[0][(int)'n'] = tmp; break;
            case 8: ditmp[0][(int)'b'] = tmp; break;
            case 9: ditmp[0][(int)'a'] = tmp; break;
            case 10: ditmp[0][(int)'c'] = tmp; break;
            case 11: ditmp[0][(int)'p'] = tmp; break;
            default: break;
        }
        ++counter;
        if(counter > 12){
            break;
        }
    }
    if(counter != 12){
        divalid = false;
    }
    if(divalid){
        ai.reset(board_pointer->turn ? NEWREDDI : NEWBLACKDI);
    }else{
        ai.reset(board_pointer->turn ? NEWRED : NEWBLACK);
    }
    return ai -> Think(depth, src, dst, valdict);
}

py::tuple God::AIHint(int depth, py::list di){
    unsigned char src = 0, dst = 0;
    py::dict valdict;
    bool thinkres = __InnerHint(depth, src, dst, &valdict, di);
    return py::make_tuple(py::int_(src), py::int_(dst), py::bool_(thinkres), valdict);
}

std::string God::Hint(int depth, py::list di){
    unsigned char src = 0, dst = 0;
    __InnerHint(depth, src, dst, NULL, di);
    return translate_ucci(src, dst);
}

short God::Rough(float discount_factor){
    return board_pointer -> Rough(discount_factor);
}


