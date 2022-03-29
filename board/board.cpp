#include "board.h"

const int board::Board::_chess_board_size = CHESS_BOARD_SIZE;

#if !DEBUG
const char board::Board::_initial_state[MAX] = 
                    "                "
                    "                "
                    "                "
                    "   defgkgfed    "
                    "   .........    "
                    "   .h.....h.    "
                    "   i.i.i.i.i    "
                    "   .........    "
                    "   .........    "
                    "   I.I.I.I.I    "
                    "   .H.....H.    "
                    "   .........    "
                    "   DEFGKGFED    "
                    "                "
                    "                "
                    "                ";
#else
const char board::Board::_initial_state[MAX] = 
                    "                "
                    "                "
                    "                "
                    "   d..gk....    "
                    "   ....a....    "
                    "   .CC......    "
                    "   .........    "
                    "   .........    "
                    "   .........    "
                    "   .........    "
                    "   .........    "
                    "   .........    "
                    "   ...K.....    "
                    "                "
                    "                "
                    "                ";
#endif

const std::unordered_map<std::string, std::string> board::Board::uni_pieces = {
    {".", "．"},
    {"R", "\033[31m俥\033[0m"},
    {"N", "\033[31m傌\033[0m"},
    {"B", "\033[31m相\033[0m"},
    {"A", "\033[31m仕\033[0m"},
    {"K", "\033[31m帅\033[0m"},
    {"P", "\033[31m兵\033[0m"},
    {"C", "\033[31m炮\033[0m"},
    {"D", "\033[31m暗\033[0m"},
    {"E", "\033[31m暗\033[0m"},
    {"F", "\033[31m暗\033[0m"},
    {"G", "\033[31m暗\033[0m"},
    {"H", "\033[31m暗\033[0m"},
    {"I", "\033[31m暗\033[0m"},
    {"r", "车"},
    {"n", "马"},
    {"b", "象"},
    {"a", "士"},
    {"k", "将"},
    {"p", "卒"},
    {"c", "炮"},
    {"d", "暗"},
    {"e", "暗"},
    {"f", "暗"},
    {"g", "暗"},
    {"h", "暗"},
    {"i", "暗"}
};

board::Board::Board(short** pst, char** _dir) noexcept: MINGZI("RNBAKCP"),
                      finished(false),
                      turn(true),
                      round(0),
                      state(DRAW),
                      pst(pst),
                      redplaces({195, 196, 197, 198, 200, 201, 202, 203, 164, 170, 147, 149, 151, 153, 155}),
                      blackplaces({59, 58, 57, 56, 54, 53, 52, 51, 90, 84, 107, 105, 103, 101, 99}),
                      LUT(
                      {
                        {51, 'd'}, {52, 'e'}, {53, 'f'}, {54, 'g'}, {56, 'g'}, {57, 'f'}, {58, 'e'}, {59, 'd'}, {84, 'h'}, {90, 'h'}, {99, 'i'}, {101, 'i'}, {103, 'i'}, {105, 'i'}, {107, 'i'},
                        {147, 'I'}, {149, 'I'}, {151, 'I'}, {153, 'I'}, {155, 'I'}, {164, 'H'}, {170, 'H'}, {195, 'D'}, {196, 'E'}, {197, 'F'}, {198, 'G'}, {200, 'G'}, {201, 'F'}, {202, 'E'}, {203, 'D'}
                      }),
                      _dir(_dir),
                      _has_initialized(false){
    memset(state_red, 0, sizeof(state_red));
    memset(state_black, 0, sizeof(state_black));
    strncpy(state_red, _initial_state, _chess_board_size);
    strncpy(state_black, _initial_state, _chess_board_size);
    memset(_is_legal_move, false, sizeof(_is_legal_move));
    memset(legal_moves, 0, sizeof(legal_moves));
    GenRandomMap();
    hist[state_red] = false;
    initialize_di();
    _has_initialized = true;
}

board::Board::~Board(){
    _has_initialized = false;
    for(size_t i = 0; i < historymoves.size(); ++i){
        history* p = historymoves[i];
        if(p){
            delete p;
            p = NULL;
        }
    }
}

void board::Board::Reset(bool turn, std::string* board){
    std::function<std::string(int, int)> king = [](int i, int j) -> std::string {
        return "King Error: RedKing==" + std::to_string(i) + ", BlackKing==" + std::to_string(j);
    };
    std::function<std::string(int, char)> dark = [](int i, char j) -> std::string {
        return "Dark Error: Pos==" + std::to_string(i) + ", value==" + std::string(1, j);
    };
    hist.clear();
    finished = false;
    this -> turn = turn;
    round = 0;
    state = DRAW;
    memset(state_red, 0, sizeof(state_red));
    memset(state_black, 0, sizeof(state_black));
    if(board){
        std::set<int> shuaiplaces = {198, 199, 200, 182, 183, 184, 166, 167, 168}, jiangplaces = {54, 55, 56, 70, 71, 72, 86, 87, 88};
        strncpy(state_red, board -> c_str(), _chess_board_size);
        strncpy(state_black, board -> c_str(), _chess_board_size);
        if(turn){
            rotate(state_black);
        }else{
            rotate(state_red);
        }
        int K = 0, k = 0;
        for(int i = 51; i <= 203; ++i){
            if(state_red[i] == 'K'){
                ++K;
                if(shuaiplaces.find(i) == shuaiplaces.end()){
                    throw Exception{"We find K at " + std::to_string(i) + "!"};
                }
            }else if(state_red[i] == 'k'){
                ++k;
                if(jiangplaces.find(i) == jiangplaces.end()){
                    throw Exception{"We find k at " + std::to_string(i) + "!"};
                }
            }
            if(K > 1 || k > 1){
                throw Exception{king(K, k)};
            }
            if((state_red[i] >= 'D' && state_red[i] <= 'I') || (state_red[i] >= 'd' && state_red[i] <= 'i')){
                if(LUT.find(i) == LUT.end() || LUT[i] != state_red[i]){
                    throw Exception{dark(i, state_red[i])};   
                }
            }
        }
        if(K != 1 || k != 1){
            throw Exception{king(K, k)};
        }
    }else{
        strncpy(state_red, _initial_state, _chess_board_size);
        strncpy(state_black, _initial_state, _chess_board_size);
    }
    state_red[_chess_board_size] = '\0';
    state_black[_chess_board_size] = '\0';
    memset(_is_legal_move, false, sizeof(_is_legal_move));
    GenRandomMap();
    hist[state_red] = false;
    initialize_di();
}

void board::Board::initialize_di(){
    memset(this -> di, 0, sizeof(this -> di));
    for(int pos: redplaces){
        if(random_map[true].find(pos) == random_map[true].end()){
            continue;
        }
        di[1][(int)random_map[true][pos]] += (state_red[pos] >= 'D' && state_red[pos] <= 'I') ? 1 : 0;
    }
    for(int pos: blackplaces){
        if(random_map[true].find(pos) == random_map[true].end()){
            continue;
        }
        di[0][(int)random_map[true][pos]] += (state_red[pos] >= 'd' && state_red[pos] <= 'i') ? 1 : 0;
    }
    memmove(this -> di_red, this -> di, sizeof(this -> di));
    memmove(this -> di_black, this -> di, sizeof(this -> di));
}

const std::vector<std::string>& board::Board::GetHistory() const{
    return _board_history;
}

std::vector<std::string> board::Board::GetStateString() const{
    std::string tmp_red(state_red);
    tmp_red = tmp_red.substr(0, board::Board::_chess_board_size);
    std::string tmp_black(state_black);
    tmp_black = tmp_black.substr(0, board::Board::_chess_board_size);
    return (std::vector<std::string>){tmp_red, tmp_black};
}


std::tuple<int, bool, std::string, std::string> board::Board::GetTuple() const{
    std::string tmp_red(state_red);
    tmp_red = tmp_red.substr(0, board::Board::_chess_board_size);
    std::string tmp_black(state_red);
    tmp_black = tmp_black.substr(0, board::Board::_chess_board_size);
    std::tuple<int, bool, std::string, std::string> ret(round, turn, tmp_red, tmp_black);
    return ret;
}

const std::unordered_map<std::string, std::string>& board::Board::GetUniPieces() const{
    return uni_pieces;
}

void board::Board::PrintPos(bool turn, bool iscovered=true, bool god=false, bool swapcasewhenblack=false) const{
    if(turn){
        if(god)
            printf("上帝视角(红):\n");
        else
            printf("红方视角:\n");
    }else{
        if(god)
            printf("上帝视角(黑):\n");
        else
            printf("黑方视角:\n");
    }
    std::cout << std::endl << std::endl;
    for(int x = 3; x <= 12; ++x){
        std::cout << translate_x(x) << " ";
        for(int y = 3; y <= 11; ++y){
            std::cout << _getstringxy(x, y, turn, iscovered, swapcasewhenblack);
        }
        std::cout << std::endl;
    }
    std::cout << "  ａｂｃｄｅｆｇｈｉ\n\n";
}

void board::Board::PrintPos() const{
    PrintPos(turn, true, false, true);
}

std::shared_ptr<InfoDict> board::Board::Move(const std::string ucci, const bool check){
    //the ucci string is in "a0a1“ format.
    //Please check https://www.xqbase.com/protocol/cchess_ucci.htm
    assert(ucci.size() == 4);
    const int y1 = (int)(ucci[0] - 'a');
    const int x1 = (int)(ucci[1] - '0');
    const int y2 = (int)(ucci[2] - 'a');
    const int x2 = (int)(ucci[3] - '0');
    return Move(translate_x_y(x1, y1), translate_x_y(x2, y2), check);
}

std::shared_ptr<InfoDict> board::Board::Move(const int encode_from, const int encode_to, const bool check){
    if(finished){
        return nullptr;
    }
    historymoves.push_back(new history(state_red, state_black, di_red, di_black));
    int reverse_encode_from = reverse(encode_from);
    int reverse_encode_to = reverse(encode_to);
    char eat = '.', eat_check = '.';
    int eat_type = 0, eat_type_tmp = 0;

    if(check) {
        if(_is_legal_move[encode_from][encode_to] == false){
            return std::shared_ptr<InfoDict>(new InfoDict(false, false, eat, eat_type, eat_check));
        }
    }

    if(turn){
        eat = state_red[encode_to];
        char eat_tmp = eat;
        FIND(eat_tmp, encode_to, turn);
        eat_check = eat_tmp;
        eat_type = eat_type_tmp;
        di_red[0][(int)eat_check] -= 1;
        state_red[encode_to] = state_red[encode_from];
        FIND(state_red[encode_to], encode_from, turn);
        if(eat_type_tmp == 2){
            di_red[1][(int)state_red[encode_to]] -= 1; 
            di_black[1][(int)state_red[encode_to]] -= 1;
        }
        state_red[encode_from] = '.';
        state_black[reverse_encode_to] = state_black[reverse_encode_from];
        FIND(state_black[reverse_encode_to], reverse_encode_from, !turn);
        assert(swapcase(state_black[reverse_encode_to]) == state_red[encode_to]);
        state_black[reverse_encode_from] = '.';
    } else{
        eat = state_black[encode_to];
        char eat_tmp = eat;
        FIND(eat_tmp, encode_to, turn);
        eat_check = eat_tmp;
        eat_type = eat_type_tmp;
        //黑吃红暗子减di_black
        di_black[1][(int)swapcase(eat_check)] -= 1;
        state_black[encode_to] = state_black[encode_from];
        FIND(state_black[encode_to], encode_from, turn);
        if(eat_type_tmp == 2){
            di_red[0][(int)swapcase(state_black[encode_to])] -= 1; 
            di_black[0][(int)swapcase(state_black[encode_to])] -= 1;
        }
        state_black[encode_from] = '.';
        state_red[reverse_encode_to] = state_red[reverse_encode_from];
        FIND(state_red[reverse_encode_to], reverse_encode_from, !turn);
        assert(swapcase(state_black[encode_to]) == state_red[reverse_encode_to]);
        state_red[reverse_encode_from] = '.';
    }
    finished = (eat == 'k');
    std::shared_ptr<InfoDict> p(new InfoDict(true, finished, eat, eat_type, eat_check));
    if(finished){
        state = turn ? REDWIN : BLACKWIN;
    }
    turn = !turn;
    hist[state_red] = turn;
    if(turn){
       ++round;
    }
    return p;
}

void board::Board::UndoMove(){
    if(!historymoves.empty()){
        history* p = historymoves.back();
        if(p){
            p -> recover(state_red, state_black, di_red, di_black);
            delete p;
        }
        historymoves.pop_back();
    }
}

void board::Board::DebugDI(){
    printf("红方视角:\n");
    for(char c: MINGZI){
        if(c == 'K') {
            continue;
        }
        printf("红方字符=%c, 红方相应个数: %d, 黑方字符=%c, 黑方相应个数=%d\n", c, di_red[1][(int)c], swapcase(c), di_red[0][(int)swapcase(c)]);
    }
    printf("黑方视角:\n");
    for(char c: MINGZI){
        if(c == 'K') {
            continue;
        }
        printf("红方字符=%c, 红方相应个数: %d, 黑方字符=%c, 黑方相应个数=%d\n", c, di_black[1][(int)c], swapcase(c), di_black[0][(int)swapcase(c)]);
    }
}

py::list board::Board::GenMoves(bool withpython){
    memset(_is_legal_move, false, sizeof(_is_legal_move));
    const char *_state_pointer = turn?state_red:state_black;
    py::list returnlist;
    for(unsigned char i = 51; i <= 203; ++i){
        const char p = _state_pointer[i];
        int intp = (int)p;
        if(!isupper(p) || p == 'U') {
            continue;
        }

        else if(p == 'C' || p == 'H') {
            for(unsigned char cnt = 0; cnt < 8; ++cnt){
                if(_dir[intp][cnt] == 0) {
                    break;
                }
                char cfoot = 0;
                for(unsigned char j = i + _dir[intp][cnt];; j += _dir[intp][cnt]) {
                    const char q = _state_pointer[j];
                    if(q == ' '){
                        break;
                    }
                    if(cfoot == 0){
                        if(q == '.'){
                            _is_legal_move[(int)i][(int)j] = true;
                            if(withpython){ 
                                returnlist.append(py::make_tuple(py::int_(i), py::int_(j)));
                            }
                        } else{
                            ++cfoot;
                        }
                    }else{
                        if(islower(q)) {
                            _is_legal_move[(int)i][(int)j] = true;
                            if(withpython) {
                                returnlist.append(py::make_tuple(py::int_(i), py::int_(j)));
                            }
                            break;
                        } else if(isupper(q)) {
                            break;
                        }
                    } //cfoot
                }//j
            }//dir
            continue;
        } //C, H

        else if(p == 'K'){
            for(unsigned char scanpos = i - 16; scanpos > A9; scanpos -= 16){
                if(_state_pointer[scanpos] == 'k'){
                    _is_legal_move[(int)i][(int)scanpos] = true;
                    if(withpython) {
                        returnlist.append(py::make_tuple(py::int_(i), py::int_(scanpos)));
                    }
                } else if(_state_pointer[scanpos] != '.'){
                    break;
                }
            }
        }

        //For other kinds of chesses
        for(unsigned char cnt = 0; cnt < 8; ++cnt){
            if(_dir[intp][cnt] == 0) {
                break;
            }
            const char d = _dir[intp][cnt];
            for(unsigned char j = i + d ;; j += d) {
                const char q = _state_pointer[j];
                if(q == ' ' || isupper(q)){
                    break;
                }
                if(i > 128 && p == 'P' && (d == EAST || d == WEST)) {
                    break;
                }
                else if(p == 'K' && (j < 160 || (j & 15) > 8 || (j & 15) < 6)) {
                    break;
                }
                else if(p == 'G' && j != 183) {
                    break;
                }
                else if(p == 'E' || p == 'N'){
                    int n_diff_x = ((int)(j - i)) & 15;
                    if(n_diff_x == 2 || n_diff_x == 14){
                        if(_state_pointer[i + (n_diff_x == 2?1:-1)] != '.'){
                           break;
                        }
                    } else{
                        if(j > i && _state_pointer[i + 16] != '.') {
                            break;
                        }
                        if(j < i && _state_pointer[i - 16] != '.') {
                            break;
                        }
                    }
                }
                else if((p == 'B' || p == 'F') && _state_pointer[i + d/2] != '.') {
                    break;
                }
                _is_legal_move[(int)i][(int)j] = true;
                if(withpython){
                    returnlist.append(py::make_tuple(py::int_(i), py::int_(j)));  
                } 
                if((p != 'D' && p != 'H' && p != 'C' && p != 'R') || islower(q)){
                    break;
                }
            } //j
        } //dir
    } //for
    return returnlist;
}//GenMoves()

std::string board::Board::GenRandomMove(){
    std::vector<std::pair<unsigned char, unsigned char>> legal_moves;
    for(unsigned char i = 51; i <= 203; ++i){
        for(unsigned char j = 51; j <= 203; ++j){
            if(_is_legal_move[(int)i][(int)j]){
                legal_moves.push_back({i, j});
            }
        }
    }
    srand(time(NULL));
    if(legal_moves.empty()) {
        return "";
    }
    size_t i = rand() % legal_moves.size();
    unsigned char first = legal_moves[i].first, second = legal_moves[i].second; 
    return translate_ucci(first, second);
}


void board::Board::Translate(unsigned char i, unsigned char j, char ucci[5]){
    int x1 = 12 - (i >> 4);
    int y1 = (i & 15) - 3;
    int x2 = 12 - (j >> 4);
    int y2 = (j & 15) - 3;
    ucci[0] = 'a' + y1;
    ucci[1] = '0' + x1;
    ucci[2] = 'a' + y2;
    ucci[3] = '0' + x2;
    ucci[4] = '\0';
}

void board::Board::TranslateSingle(unsigned char i, char ucci[3]){
    int x1 = 12 - (i >> 4);
    int y1 = (i & 15) - 3;
    ucci[0] = 'a' + y1;
    ucci[1] = '0' + x1;
    ucci[2] = '\0';
}


void board::Board::GenRandomMap(){
    std::multiset<char> chararray_red_set = {'R', 'R', 'N', 'N', 'B', 'B', 'A', 'A', 'C', 'C', 'P', 'P', 'P', 'P', 'P'};
    std::multiset<char> chararray_black_set = {'r', 'r', 'n', 'n', 'b', 'b', 'a', 'a', 'c', 'c', 'p', 'p', 'p', 'p', 'p'};
    for(int i = 51; i <= 203; ++i){
        if(::isupper(state_red[i]) && state_red[i] != 'K' && MINGZI.find(state_red[i]) != std::string::npos){
            auto it = chararray_red_set.lower_bound(state_red[i]);
            if(it != chararray_red_set.end()) {chararray_red_set.erase(it);}
        }
        if(::islower(state_red[i]) && state_red[i] != 'k' && MINGZI.find(swapcase(state_red[i])) != std::string::npos){
            auto it = chararray_black_set.lower_bound(state_red[i]);
            if(it != chararray_black_set.end()) {chararray_black_set.erase(it);}
        }
    }
    std::vector<char> chararray_red, chararray_black;
    chararray_red.assign(chararray_red_set.begin(), chararray_red_set.end());
    chararray_black.assign(chararray_black_set.begin(), chararray_black_set.end());
    std::vector<unsigned char> position_red = {TXY(0, 0), TXY(0, 1), TXY(0, 2), TXY(0, 3), TXY(0, 5), TXY(0, 6), \
        TXY(0, 7), TXY(0, 8), TXY(2, 1), TXY(2, 7), TXY(3, 0), TXY(3, 2), TXY(3, 4), TXY(3, 6), TXY(3, 8)};
    std::vector<unsigned char> position_black = {TXY(9, 0), TXY(9, 1), TXY(9, 2), TXY(9, 3), TXY(9, 5), TXY(9, 6), \
        TXY(9, 7), TXY(9, 8), TXY(7, 1), TXY(7, 7), TXY(6, 0), TXY(6, 2), TXY(6, 4), TXY(6, 6), TXY(6, 8)};
    int size = 15;
    SV(chararray_red); 
    SV(chararray_black);
    std::unordered_map<unsigned char, char> r, b;
    for(int i = 0, cnt = 0; i < size; ++i){
        if(state_red[position_red[i]] >= 'D' && state_red[position_red[i]] <= 'I'){
            r[position_red[i]] = chararray_red[cnt];
            b[reverse(position_red[i])] = swapcase(chararray_red[cnt]);
            ++cnt;
        }else{
            continue;
        }
    }
    for(int i = 0, cnt = 0; i < size; ++i){
        if(state_red[position_black[i]] >= 'd' && state_red[position_black[i]] <= 'i'){
            r[position_black[i]] = chararray_black[cnt]; 
            b[reverse(position_black[i])] = swapcase(chararray_black[cnt]);
            ++cnt;
        }else{
            continue;
        }
    }
    random_map[true] = r;
    random_map[false] = b;
}

void board::Board::PrintRandomMap(){
    for(std::unordered_map<unsigned char, char>::iterator it = random_map[true].begin(); it != random_map[true].end(); ++it){
        std::cout << (translate_single(it -> first) + ": " + it -> second) << "\n";
    }
}

void board::Board::GenRandomBoard(bool turn){
    finished = false;
    this -> turn = turn;
    round = 0;
    state = DRAW;
    srand(time(NULL));
    memset(state_red, ' ', sizeof(state_red));
    memset(state_black, ' ', sizeof(state_black));
    random_map.clear();
    std::multiset<char> chararray_red_set = {'R', 'R', 'N', 'N', 'B', 'B', 'A', 'A', 'C', 'C', 'P', 'P', 'P', 'P', 'P'};
    std::multiset<char> chararray_black_set = {'r', 'r', 'n', 'n', 'b', 'b', 'a', 'a', 'c', 'c', 'p', 'p', 'p', 'p', 'p'};
    std::multiset<unsigned char> all_places;
    for(unsigned char i = 51; i <= 203; ++i){
        if((i & 15) < 3 || (i & 15) > 11){
            continue;
        }
        all_places.insert(i);
        state_red[i] = '.';
    }
    for(std::unordered_map<unsigned char, char>::iterator it = LUT.begin(); it != LUT.end(); ++it){
        unsigned char pos = it -> first;
        char c = it -> second;
        char random_char = ' ';
        if(rand() % 2){
            if(::isupper(c) && !chararray_red_set.empty()){
                random_char = select_random(chararray_red_set, true); 
            }else if(::islower(c) && !chararray_black_set.empty()){
                random_char = select_random(chararray_black_set, true);
            }
        }
        if(random_char != ' '){
            state_red[pos] = c;
            random_map[true][pos] = random_char;
            all_places.erase(pos);
        }
    }
    std::multiset<unsigned char> shuaiplaces = {198, 199, 200, 182, 183, 184, 166, 167, 168}, jiangplaces = {54, 55, 56, 70, 71, 72, 86, 87, 88};
    unsigned char shuai = select_random(shuaiplaces, false);
    unsigned char jiang = select_random(jiangplaces, false);
    state_red[shuai] = 'K'; state_red[jiang] = 'k';
    all_places.erase(shuai); all_places.erase(jiang);
    for(char red: chararray_red_set){
        if(rand() % 2 && !all_places.empty()){
            unsigned char pos = select_random(all_places, true);
            state_red[(int)pos] = red;   
        }
    }
    for(char black: chararray_black_set){
        if(rand() % 2 && !all_places.empty()){
            unsigned char pos = select_random(all_places, true);
            state_red[(int)pos] = black;
        }
    }
    memmove(state_black, state_red, sizeof(state_red));
    rotate(state_black);
    for(std::unordered_map<unsigned char, char>::iterator it = random_map[true].begin(); it != random_map[true].end(); it++){
        random_map[false][254 - (it -> first)] = swapcase(random_map[true][it -> first]);
    }
    initialize_di();
} 

short board::Board::Rough(float discount_factor){
    short result = 0;
    float che = 0;
    float che_opponent = 0;
    const char* p = turn ? state_red : state_black;
    for(unsigned char i = 51; i <= 203; ++i){
        if((i & 15) < 3 || (i & 15) > 11){
            continue;
        }
        if(p[i] >= 'D' && p[i] <= 'I'){
            int real_chess = (int)random_map[turn][i];
            if(real_chess == 'R'){
                che += 0.5f;
            }
            result += (short)(pst[real_chess][i]/discount_factor);
        }
        else if(::isupper(p[i])){
            if(p[i] == 'R'){
                che += 1.0f;
            }
            result += pst[(int)p[i]][i];
        }
        else if(p[i] >= 'd' && p[i] <= 'i'){
            int real_chess = (int)random_map[turn][i];
            if(real_chess == 'r'){
                che_opponent += 0.5f;
            }
            result -= (short)(pst[(int)swapcase(real_chess)][254-i]/discount_factor);
        }
        else if(::islower(p[i])){
            if(p[i] == 'r'){
                che_opponent += 1.0f;
            }
            result -= pst[(int)swapcase(p[i])][254-i];
        }
    }
    if(p[195] != 'D'){
        result -= (short)(30 * che_opponent);
    }
    if(p[203] != 'D'){
        result -= (short)(30 * che_opponent);
    }
    if(p[51] != 'd'){
        result += (short)(30 * che);
    }
    if(p[59] != 'd'){
        result += (short)(30 * che);
    }
    return result;
}