#include "aiboard4.h"


std::unordered_map<int, char> LUT4 = {
   {195, 'D'},
   {196, 'E'},
   {197, 'F'},
   {198, 'G'},
   {200, 'G'},
   {201, 'F'},
   {202, 'E'},
   {203, 'D'},
   {164, 'H'},
   {170, 'H'},
   {147, 'I'},
   {149, 'I'},
   {151, 'I'},
   {153, 'I'},
   {155, 'I'}
};

const int board::AIBoard4::_chess_board_size = CHESS_BOARD_SIZE;
const char board::AIBoard4::_initial_state[MAX] = 
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



const std::unordered_map<std::string, std::string> board::AIBoard4::_uni_pieces = {
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
    {"U", "\033[31m不\033[0m"},
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
    {"i", "暗"},
    {"u", "不"},
};

std::unordered_map<std::string, SCORE4> score_bean4;
std::unordered_map<std::string, KONGTOUPAO_SCORE4> kongtoupao_score_bean4;
std::unordered_map<std::string, THINKER4> thinker_bean4;


board::AIBoard4::AIBoard4(const char another_state[MAX], bool turn, int round, const unsigned char di[2][123], short score, std::unordered_map<std::string, bool>* hist, \
    std::unordered_map<std::pair<uint32_t, bool>, std::pair<unsigned char, unsigned char>, myhash<uint32_t, bool>>* tp_move_bean, \
    std::unordered_map<std::pair<uint32_t, int>, std::pair<short, short>, myhash<uint32_t, int>>* tp_score_bean) noexcept:  lastinsert(false),
                                                                                                                            version(0), 
                                                                                                                            round(round), 
                                                                                                                            turn(turn), 
                                                                                                                            original_turn(turn),
                                                                                                                            zobrist_hash(0), 
                                                                                                                            discount_factor(1.5),
                                                                                                                            score(score),
                                                                                                                            tp_move(tp_move_bean),
                                                                                                                            tp_score(tp_score_bean),
                                                                                                                            hist(hist),
                                                                                                                            _myname("AI4"),
                                                                                                                            _has_initialized(false),
                                                                                                                            _score_func(NULL),
                                                                                                                            _kongtoupao_score_func(NULL){
    SetScoreFunction("complicated_score_function4", 0);
    SetScoreFunction("complicated_kongtoupao_score_function4", 1);
    score_cache.push(score);
    memset(state_red, 0, sizeof(state_red));
    memset(state_black, 0, sizeof(state_black));
    strncpy(state_red, another_state, _chess_board_size);
    strncpy(state_black, another_state, _chess_board_size);
    if(turn){
        rotate(state_black);
    }else{
        rotate(state_red);
    }
    CopyData(di);
    _initialize_zobrist();
    zobrist_cache.insert((zobrist_hash << 1)|original_turn);
    Scan();
    register_score_functions4();
    _has_initialized = true;
}

void board::AIBoard4::SetScoreFunction(std::string function_name, int type){
    if(type == 0){
        _score_func = GetWithDefUnordered<std::string, SCORE4>(score_bean4, function_name, complicated_score_function4);
    }else if(type == 1){
        _kongtoupao_score_func = GetWithDefUnordered<std::string, KONGTOUPAO_SCORE4>(kongtoupao_score_bean4, function_name, complicated_kongtoupao_score_function4);
    }else if(type == 2){
        _thinker_func = GetWithDefUnordered<std::string, THINKER4>(thinker_bean4, function_name, mtd_thinker4);
    }
}

std::string board::AIBoard4::SearchScoreFunction(int type){
    if(type == 0){
        return ::SearchScoreFunction4(reinterpret_cast<void*>(_score_func), type);
    }else if(type == 1){
        return ::SearchScoreFunction4(reinterpret_cast<void*>(_kongtoupao_score_func), type);
    }else if(type == 2){
        return ::SearchScoreFunction4(reinterpret_cast<void*>(_thinker_func), type);
    }
    return "";
}

bool board::AIBoard4::Move(const unsigned char encode_from, const unsigned char encode_to, short score_step){
    const unsigned char reverse_encode_from = reverse(encode_from);
    const unsigned char reverse_encode_to = reverse(encode_to);
    if(turn){
        cache.push({encode_from, encode_to, state_red[encode_to]});
        zobrist_hash ^= zobrist[(int)state_red[encode_to]][encode_to];
        zobrist_hash ^= zobrist[(int)state_red[encode_from]][encode_from];
        if(state_red[encode_from] >= 'D' && state_red[encode_from] <= 'I'){
            state_red[encode_to] = 'U';
            state_red[encode_from] = '.';
            state_black[reverse_encode_to] = 'u';
            state_black[reverse_encode_from] = '.';
        }else{
            state_red[encode_to] = state_red[encode_from];
            state_red[encode_from] = '.';
            state_black[reverse_encode_to] = state_black[reverse_encode_from];
            state_black[reverse_encode_from] = '.';
        }
        zobrist_hash ^= zobrist[(int)state_red[encode_to]][encode_to];
    } else{
        cache.push({encode_from, encode_to, state_black[encode_to]});
        zobrist_hash ^= zobrist[(int)state_red[reverse_encode_to]][reverse_encode_to];
        zobrist_hash ^= zobrist[(int)state_red[reverse_encode_from]][reverse_encode_from];
        if(state_black[encode_from] >= 'D' && state_black[encode_from] <= 'I'){
            state_black[encode_to] = 'U';
            state_black[encode_from] = '.';
            state_red[reverse_encode_to] = 'u';
            state_red[reverse_encode_from] = '.';
        }else{
            state_black[encode_to] = state_black[encode_from];
            state_black[encode_from] = '.';
            state_red[reverse_encode_to] = state_red[reverse_encode_from];
            state_red[reverse_encode_from] = '.';
        }
        zobrist_hash ^= zobrist[(int)state_red[reverse_encode_to]][reverse_encode_to];
    }
    turn = !turn;
    score = -(score + score_step);
    if(turn){
       ++round;
    }
    score_cache.push(score);
    auto zobrist_turn = (zobrist_hash << 1)|turn;
    bool retval = (zobrist_cache.find(zobrist_turn) == zobrist_cache.end());
    if(retval){
        zobrist_cache.insert(zobrist_turn);
        Scan();
    }
    lastinsert = retval;
    return retval;
}

void board::AIBoard4::NULLMove(){
    turn = !turn;
    zobrist_cache.insert((zobrist_hash << 1)|turn);
    score = -score;
    score_cache.push(score);
    Scan();
}

void board::AIBoard4::UndoMove(int type){
    score_cache.pop();
    score = score_cache.top();
    if(lastinsert){
        zobrist_cache.erase((zobrist_hash<<1)|turn);
    }
    if(type == 1){//非空移动
        const std::tuple<unsigned char, unsigned char, char> from_to_eat = cache.top();
        cache.pop();
        const unsigned char encode_from = std::get<0>(from_to_eat);
        const unsigned char encode_to = std::get<1>(from_to_eat);
        const char eat = std::get<2>(from_to_eat);
        if(turn){
            --round;
        }
        turn = !turn;
        const unsigned char reverse_encode_from = reverse(encode_from);
        const unsigned char reverse_encode_to = reverse(encode_to);
        if(turn){
            zobrist_hash ^= zobrist[(int)state_red[encode_to]][encode_to];
            if(state_red[encode_to] == 'U'){
                state_red[encode_from] = LUT4[encode_from];
                state_red[encode_to] = eat;
                state_black[reverse_encode_from] = swapcase(LUT4[encode_from]);
                state_black[reverse_encode_to] = swapcase(eat);
            }else{
                state_red[encode_from] = state_red[encode_to];
                state_red[encode_to] = eat;
                state_black[reverse_encode_from] = state_black[reverse_encode_to];
                state_black[reverse_encode_to] = swapcase(eat);
            }
            zobrist_hash ^= zobrist[(int)state_red[encode_from]][encode_from];
            zobrist_hash ^= zobrist[(int)state_red[encode_to]][encode_to];
        }else{
            zobrist_hash ^= zobrist[(int)state_red[reverse_encode_to]][reverse_encode_to];
            if(state_black[encode_to] == 'U'){
                state_black[encode_from] = LUT4[encode_from];
                state_black[encode_to] = eat;
                state_red[reverse_encode_from] = swapcase(LUT4[encode_from]);
                state_red[reverse_encode_to] = swapcase(eat);
            }else{
                state_black[encode_from] = state_black[encode_to];
                state_black[encode_to] = eat;
                state_red[reverse_encode_from] = state_red[reverse_encode_to];
                state_red[reverse_encode_to] = swapcase(eat);
            }
            zobrist_hash ^= zobrist[(int)state_red[reverse_encode_from]][reverse_encode_from];
            zobrist_hash ^= zobrist[(int)state_red[reverse_encode_to]][reverse_encode_to];
        }
        //Scan(); //这里不需要再Scan，因为Scan是用来统计移动分数的和Quiescence的，Move之前就应该已经统计完成
    }else if(type == 0){
        turn = !turn;
    }
}

void board::AIBoard4::Scan(){
    all = 0;
    che = 0;
    che_opponent = 0;
    zu = 0;
    covered = 0;
    covered_opponent = 0;
    endline = 0;
    score_rough = 0;
    kongtoupao = 0;
    kongtoupao_opponent = 0;
    kongtoupao_score = 0;
    kongtoupao_score_opponent=0;
    const char *_state_pointer = turn?state_red:state_black;
    for(int i = 51; i <= 203; ++i){
        if((i & 15) < 3 || (i & 15) > 11) { continue; }
        const char p = _state_pointer[i];
        if(p != '.'){
            ++all;
        }
        if(p == 'R' || p == 'N' || p == 'B' || p == 'A' || p == 'K' || p == 'C' || p == 'P'){
            score_rough += pst[(int)p][i];
            if(p == 'R'){
               ++che;
            }else if(p == 'P'){
               ++zu;
            }
        }
        else if(p >= 'D' && p <= 'I'){
            ++covered;
        }
        else if(p == 'U'){
            score_rough += aiaverage[version][turn?1:0][1][i];
            ++covered;
        }
        else if(p == 'r' || p == 'n' || p == 'b' || p == 'a' || p == 'k' || p == 'c' || p == 'p'){
            score_rough -= pst[((int)p) ^ 32][254 - i];
            if(p == 'r'){
               ++che_opponent;
            }
        }
        else if(p >= 'd' && p <= 'i'){
            ++covered_opponent;
        }  
        else if(p == 'u'){
            score_rough -= aiaverage[version][turn?0:1][1][254 - i];
            ++covered_opponent;
        }
        if(p == 'C' && ((i & 15) == 7)){
            KongTouPao(_state_pointer, i, true);
        }
        if(p == 'c' && ((i & 15) == 7)){
            KongTouPao(_state_pointer, i, false);
        }
    }
    _kongtoupao_score_func(this, &kongtoupao_score, &kongtoupao_score_opponent);
}

short board::AIBoard4::ScanProtectors(){
    const char *_state_pointer = turn?state_red:state_black;
    protector = 4;
    protector_oppo = 4;
    if(_state_pointer[195] != 'D'){
        --protector;
    }
    if(_state_pointer[203] != 'D'){
        --protector;
    }
    if(_state_pointer[198] != 'G' && _state_pointer[198] != 'A'){
        --protector;
    }
    if(_state_pointer[200] != 'G' && _state_pointer[200] != 'A'){
        --protector;
    }
    if(_state_pointer[183] == 'A'){
        ++protector;
    }
    if(_state_pointer[51] != 'd'){
        --protector_oppo;
    }
    if(_state_pointer[59] != 'd'){
        --protector_oppo;
    }
    if(_state_pointer[54] != 'g' && _state_pointer[54] != 'a'){
        --protector_oppo;
    }
    if(_state_pointer[56] != 'g' && _state_pointer[56] != 'a'){
        --protector_oppo;
    }
    if(_state_pointer[71] == 'a'){
        ++protector_oppo;
    }
    unsigned char myself = protector >= 4 ? 0 : 4 - protector;
    unsigned char oppo = protector_oppo >= 4 ? 0 : 4 - protector_oppo;
    unsigned char penaltyarea[20] = {197, 198, 199, 200, 201, 181, 182, 183, 184, 185, 165, 166, 167, 168, 169, 149, 150, 151, 152, 153};
    unsigned char penaltyareaoppo[20] = {53, 54, 55, 56, 57, 69, 70, 71, 72, 73, 85, 86, 87, 88, 89, 101, 102, 103, 104, 105};
    short bonus = 0;
    const short delta = 10;
    for(int i = 0; i < 20; ++i){
        if(_state_pointer[penaltyareaoppo[i]] == 'P'){
            short tmp = (delta * (4 - L1[penaltyareaoppo[i]][71]) * oppo);
            if(penaltyareaoppo[i] <= 57){
                //底兵
                tmp /= 2;
            }
            assert(tmp >= 0);
            bonus += tmp;
        }
        if(_state_pointer[penaltyarea[i]] == 'p'){
            short tmp = (delta * (4 - L1[penaltyarea[i]][183]) * myself);
            if(penaltyarea[i] >= 197){
                tmp /= 2;
            }
            assert(tmp >= 0);
            bonus -= tmp;
        }
    }
    return bonus;
}

void board::AIBoard4::KongTouPao(const char* _state_pointer, int pos, bool myself){
    char cannon = myself?'C':'c';
    char king = myself?'k':'K';
    if(myself){
        if(kongtoupao != 0){
            return;
        } 
        for(int scanpos = pos - 16; scanpos > A9; scanpos -= 16){
            char p = _state_pointer[scanpos];
            if(p == cannon){
                continue;
            }
            else if(p != '.'){
                if(p == king){
                    ++kongtoupao;
                }else{
                    kongtoupao = 0;
                }
                return;
            }else{
                ++kongtoupao; //Python版本 (musesfish_pvs_20210604_fixed.py)没有这一行, Python BUG!
            } 
        }
        //kongtoupao = 0; //注释这行, 即使出将也不能化解空头炮
        return;
    }else{
        if(kongtoupao_opponent != 0){
            return;
        } 
        for(int scanpos = pos + 16; scanpos < I0; scanpos += 16){
            char p = _state_pointer[scanpos];
            if(p == cannon){
                continue;
            }
            else if(p != '.'){
                if(p == king){
                    ++kongtoupao_opponent;
                }else{
                    kongtoupao_opponent = 0;
                }
                return;
            }else{
                ++kongtoupao_opponent; //Python (musesfish_pvs_20210604_fixed.py)版本没有这一行, BUG!
            } 
        }
        //kongtoupao_opponent = 0; //注释这行, 即使出将也不能化解空头炮
        return;
    } //else
} //KongTouPao

template<bool needscore, bool return_after_mate>
bool board::AIBoard4::GenMovesWithScore(std::tuple<short, unsigned char, unsigned char> legal_moves[MAX_POSSIBLE_MOVES], int& num_of_legal_moves, std::pair<unsigned char, unsigned char>* killer, short& killer_score, unsigned char& mate_src, unsigned char& mate_dst, bool& killer_is_alive){
    num_of_legal_moves = 0;
    killer_score = 0;
    bool mate = false;
    killer_is_alive = false;
    const char *_state_pointer = turn?state_red:state_black;
    for(unsigned char i = 51; i <= 203; ++i){
        if((i & 15) < 3 || (i & 15) > 11) { continue; }
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
                            short score_tmp = 0;
                            if(needscore){
                                score_tmp = _score_func(this, _state_pointer, i, j);
                            }
                            legal_moves[num_of_legal_moves] = std::make_tuple(score_tmp, i, j);
                            if(killer && killer -> first == i && killer -> second == j && needscore){
                                killer_score = score_tmp;
                                killer_is_alive = true;
                            }
                            ++num_of_legal_moves;
                        } else{
                            ++cfoot;
                        }
                    }else{
                        if(islower(q)) {
                            short score_tmp = 0;
                            if(needscore){
                                score_tmp = _score_func(this, _state_pointer, i, j);
                            }
                            legal_moves[num_of_legal_moves] = std::make_tuple(score_tmp, i, j);
                            if(killer && killer -> first == i && killer -> second == j && needscore){
                                killer_score = score_tmp;
                                killer_is_alive = true;
                            }
                            ++num_of_legal_moves;
                            if(q == 'k') { 
                                mate = true; 
                                mate_src = i, mate_dst = j;
                                if(return_after_mate){ return true; }
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
                    short score_tmp = 0;
                    if(needscore){
                        score_tmp = _score_func(this, _state_pointer, i, scanpos);
                    }
                    legal_moves[num_of_legal_moves] = std::make_tuple(score_tmp, i, scanpos);
                    if(killer && killer -> first == i && killer -> second == scanpos && needscore){
                        killer_score = score_tmp;
                        killer_is_alive = true;
                    }
                    ++num_of_legal_moves;
                    mate = true;
                    mate_src = i, mate_dst = scanpos;
                    if(return_after_mate){ return true; }
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
            if(i > 128 && p == 'P' && (d == EAST || d == WEST)) {
                break;
            }
            for(unsigned char j = i + d ;; j += d) {
                const char q = _state_pointer[j];
                if(q == ' ' || isupper(q)){
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
                short score_tmp = 0;
                if(needscore){
                    score_tmp = _score_func(this, _state_pointer, i, j);
                }
                legal_moves[num_of_legal_moves] = std::make_tuple(score_tmp, i, j);
                if(killer && killer -> first == i && killer -> second == j && needscore){
                    killer_score = score_tmp;
                    killer_is_alive = true;
                }
                if(q == 'k'){
                    mate = true;
                    mate_src = i, mate_dst = j;
                    if(return_after_mate){ return true; }
                }
                ++num_of_legal_moves;
                if((p != 'D' && p != 'R') || islower(q)){
                    break;
                }
            } //j
        } //dir
    } //for
    if(needscore){
        std::sort(legal_moves, legal_moves + num_of_legal_moves, GreaterTuple<short, unsigned char, unsigned char>);
    }
    return mate;
}//GenMovesWithScore()


template<bool doublereverse>
bool board::AIBoard4::Mate(){
    if(doublereverse)
        turn = !turn;
    std::tuple<short, unsigned char, unsigned char> legal_moves_tmp[MAX_POSSIBLE_MOVES];
    int num_of_legal_moves_tmp = 0;
    short killer_score = 0;
    unsigned char mate_src = 0, mate_dst = 0;
    bool killer_is_alive = false;
    bool mate = GenMovesWithScore<false, true>(legal_moves_tmp, num_of_legal_moves_tmp, NULL, killer_score, mate_src, mate_dst, killer_is_alive);
    if(doublereverse)
        turn = !turn;
    return mate;
}

bool board::AIBoard4::Executed(bool* oppo_mate, std::tuple<short, unsigned char, unsigned char> legal_moves_tmp[], int num_of_legal_moves_tmp, bool calc){
    //判断是否被对方将死
    //注意, 这个函数应该在mate_by_oppo为true的时候调用。如mate_by_oppo==false, 根本没将军, 调用没意义
    if(calc){
        *oppo_mate = Mate<true>();
    }
    if(!*oppo_mate){
        return false;
    }
    bool saved = false; //还有救?
    for(int i = 0; i < num_of_legal_moves_tmp; ++i){
        auto tuple = legal_moves_tmp[i];
        Move(std::get<1>(tuple), std::get<2>(tuple), std::get<0>(tuple));
        if(!Mate<false>()){
            saved = true;
        }
        UndoMove(1);
        if(saved){return false;}
    }
    return true;
}

#if DEBUG
bool board::AIBoard4::ExecutedDebugger(bool *oppo_mate){
    std::tuple<short, unsigned char, unsigned char> legal_moves_tmp[MAX_POSSIBLE_MOVES];
    int num_of_legal_moves_tmp = 0;
    short killer_score = 0;
    unsigned char mate_src = 0, mate_dst = 0;
    bool killer_is_alive = false;
    GenMovesWithScore<false, false>(legal_moves_tmp, num_of_legal_moves_tmp, NULL, killer_score, mate_src, mate_dst, killer_is_alive);
    return Executed(oppo_mate, legal_moves_tmp, num_of_legal_moves_tmp, true);
}
#endif

bool board::AIBoard4::Ismate_After_Move(unsigned char src, unsigned char dst){
    //判断本方在走完某步棋后是否对对方形成将军
    //return true: 形成将军
    //return false: 不形成将军
    //a9     a5(k)
    //a8(R)
    //a8a9后R位于a9形成将军return true
    //a8a7后不形成将军return false
    short score = 0;
    unsigned char mate_src, mate_dst;
    std::tuple<short, unsigned char, unsigned char> legal_moves_tmp[MAX_POSSIBLE_MOVES];
    int num_of_legal_moves_tmp = 0;
    bool killer_is_alive = false;
    Move(src, dst, 0);
    turn = !turn;
    bool mate = GenMovesWithScore<false, true>(legal_moves_tmp, num_of_legal_moves_tmp, NULL, score, mate_src, mate_dst, killer_is_alive);
    turn = !turn;
    UndoMove(1);
    return mate;
}

void board::AIBoard4::CalcVersion(const int ver, const float discount_factor){
    short numr = 0, numb = 0; 
    numr += aidi[ver][1][INTR]; numr += aidi[ver][1][INTN];  numr += aidi[ver][1][INTB];  numr += aidi[ver][1][INTA];  numr += aidi[ver][1][INTC]; numr += aidi[ver][1][INTP]; 
    numb += aidi[ver][0][INTr]; numb += aidi[ver][0][INTn];  numb += aidi[ver][0][INTb];  numb += aidi[ver][0][INTa];  numb += aidi[ver][0][INTc]; numb += aidi[ver][0][INTp]; 
    aisumall[ver][1] = numr; aisumall[ver][0] = numb;
    if(numr > 0){
        double sumr = 0.0;
        for(const char c : MINGZI){
            //for c == 'K', aidi[ver][1][(int)c] = 0
            sumr += pst[(int)c][0] * aidi[ver][1][(int)c] / discount_factor;
        }
        aiaverage[ver][1][0][0] = ::round(sumr / numr);
        for(int i = 51; i <= 203; ++i){
            sumr = 0.0;
            for(const char c : MINGZI){
                sumr += pst[(int)c][i] * aidi[ver][1][(int)c];
            }
            aiaverage[ver][1][1][i] = ::round(sumr / numr);
        }
    }
    if(numb > 0){
        double sumb = 0.0;
        for(const char c : MINGZI){
            sumb += pst[(int)c][0] * aidi[ver][0][((int)c)^32] / discount_factor;
        }
        aiaverage[ver][0][0][0] = ::round(sumb / numb);
        for(int i = 51; i <= 203; ++i){
            sumb = 0.0;
            for(const char c : MINGZI){
                sumb += pst[(int)c][i] * aidi[ver][0][((int)c)^32];
            }
            aiaverage[ver][0][1][i] = ::round(sumb / numb);
        }
    }
}

void board::AIBoard4::CopyData(const unsigned char di[2][123]){
    memset(aiaverage, 0, sizeof(aiaverage));
    memset(aisumall, 0, sizeof(aisumall));
    memset(aidi, 0, sizeof(aidi));
    for(int i = 0; i < VERSION_MAX; ++i){
        memcpy(aidi[i], di, sizeof(aidi[i]));
    }
    CalcVersion(0, discount_factor);
}

std::string board::AIBoard4::Think(){
    SetScoreFunction("mtd_thinker4", 2);
    return _thinker_func(this);
}

void board::AIBoard4::PrintPos(bool turn) const{
    printf("version = %d, turn = %d, this -> turn = %d, round = %d\n", version, turn, this -> turn, round);
    if(turn){
        printf("红方视角:\n");
    }else{
        printf("黑方视角:\n");
    }
    std::cout << std::endl << std::endl;
    for(int x = 3; x <= 12; ++x){
        std::cout << translate_x(x) << " ";
        for(int y = 3; y <= 11; ++y){
            std::cout << _getstringxy(x, y, turn);
        }
        std::cout << std::endl;
    }
    std::cout << "  ａｂｃｄｅｆｇｈｉ\n\n";
}

std::string board::AIBoard4::DebugPrintPos(bool turn) const{
    std::string ret;
    if(turn){
        ret += "RED\n";   
    }else{
        ret += "BLACK\n";
    }
    for(int x = 3; x <= 12; ++x){
        ret += std::to_string(translate_x(x));
        ret += ' ';
        for(int y = 3; y <= 11; ++y){
            const char c = turn?state_red[encode(x, y)]:state_black[encode(x, y)];
            ret += c;
        }
        ret += '\n';
    }
    ret += "  abcdefghi\n";
    return ret;
}

void board::AIBoard4::print_raw_board(const char* board, const char* hint){
    std::cout << hint << std::endl;
    int row = 9;
    for(int i = 51; i < 203; i += 16){
        std::cout << row << " ";
        for(int j = i; j <= i+8; ++j){
            std::cout << board[j];
        }
        --row;
        std::cout << "\n";
    }
    std::cout << "***********\n  abcdefghi\n";
}


template<typename... Args>
void board::AIBoard4::print_raw_board(const char* board, const char* hint, Args... args){
    print_raw_board(board, hint);
    print_raw_board(args...);
}

inline short complicated_score_function4(board::AIBoard4* self, const char* state_pointer, unsigned char src, unsigned char dst){
    #define LOWER_BOUND -32768
    #define UPPER_BOUND 32767
    constexpr short MATE_UPPER = 2600;
    board::AIBoard4* bp = reinterpret_cast<board::AIBoard4*>(self);
    int version = bp -> version;
    int turn = bp -> turn ? 1 : 0;
    int che_char = bp -> turn ? (int)'R': (int)'r';
    int che_opponent_char = che_char ^ 32;
    int zu_char = bp -> turn ? (int)'P': (int)'p';
    char p = state_pointer[src], q = bp -> swapcase(state_pointer[dst]);
    int intp = (int)p, intq = (int)q;
    float score = 0.0;
    float possible_che = 0.0;
    if(bp -> aisumall[version][turn]){
        possible_che = (float)((bp -> covered) * bp -> aidi[version][turn][che_char])/bp -> aisumall[version][turn];
    }
    float zu_possibility = 0.0;
    if(bp -> aisumall[version][turn]){
        zu_possibility = (float)(bp -> aidi[version][turn][zu_char])/bp -> aisumall[version][turn];
    }
    float possible_che_opponent = 0.0;
    if(bp -> aisumall[version][1 - turn]){
        possible_che_opponent = (float)((bp -> covered_opponent) * bp -> aidi[version][1 - turn][che_opponent_char])/bp -> aisumall[version][1 - turn];
    }
    if(q == 'K'){
        return MATE_UPPER;
    }
    if(p == 'R' || p == 'N' || p == 'B' || p == 'A' || p == 'K' || p == 'C' || p == 'P'){
        score =  pst[intp][dst] -  pst[intp][src];
        
        if(p == 'R'){
            if(state_pointer[51] != 'd' && state_pointer[51] != 'r' && state_pointer[54] != 'a' && state_pointer[71] != 'a' && (state_pointer[71] == 'p' || state_pointer[87] != 'n')){
                if( (dst & 15) == 6 && (src & 15) != 6 ) { score += 30; }
                else if((src & 15) == 6 && (dst & 15) != 6) { score -= 30; }
            }
            
            if(state_pointer[59] != 'd' && state_pointer[59] != 'r' && state_pointer[56] != 'a' && state_pointer[71] != 'a' && (state_pointer[71] == 'p' || state_pointer[87] != 'n')){
                if( (dst & 15) == 8 && (src & 15) != 8 ) { score += 30; }
                else if((src & 15) == 8 && (dst & 15) != 8) { score -= 30; }
            }
            
        }//else if( p == 'R')
    }
    
    else{
        score = bp -> aiaverage[version][turn][1][dst] - bp -> aiaverage[version][turn][0][0];
        float che_zu_possibility = bp -> aisumall[version][turn] > 0 ? (bp -> aidi[version][turn][che_char] + bp -> aidi[version][turn][zu_char])/bp -> aisumall[version][turn] : 0.0;
        float scorediff = bp -> aiaverage[version][turn][0][0] * che_zu_possibility;
        if(p == 'D'){
            if(bp -> score_rough < -150){
                score -= (45 * (possible_che_opponent/2 + bp -> che_opponent - (q == 'R')));
            }else{
                score -= (30 * (possible_che_opponent/2 + bp -> che_opponent - (q == 'R')));
            }
        }else if(p == 'E'){
            auto judge_left = [&state_pointer]() -> bool {
                return state_pointer[147] == 'r' || state_pointer[148] == 'r';
            };

            auto judge_middle = [&state_pointer]() -> bool {
                return state_pointer[150] == 'r' || state_pointer[151] == 'r' || state_pointer[152] == 'r';
            };

            auto judge_right = [&state_pointer]() -> bool {
                return state_pointer[154] == 'r' || state_pointer[155] == 'r';
            };

            if(src == 196 && dst == 165 && state_pointer[149] == 'I'){
                if(judge_left() || judge_middle()){
                    score += scorediff;
                }
                else{
                    for(int scanpos = 133; scanpos > A9; scanpos -= 16){
                        if(state_pointer[scanpos] == 'r'){
                            score += scorediff;
                        }else if(state_pointer[scanpos] != '.'){
                            break;
                        }                       
                    }
                }
            }   

            if(src == 202 && dst == 169 && state_pointer[153] == 'I'){
                if(judge_right() || judge_middle()){
                    score += scorediff;
                }
                else{
                    for(int scanpos = 137; scanpos > A9; scanpos -= 16){
                        if(state_pointer[scanpos] == 'r'){
                            score += scorediff;
                        }else if(state_pointer[scanpos] != '.'){
                            break;
                        } 
                    }
                }
            }
        
        
        }else if(p == 'F'){
            if((src == 197 || src == 201) && dst == 167 && state_pointer[151] == 'I'){
                bool findche = false;
                for(int scanpos = 135; scanpos > A9; scanpos -= 16){
                    if(state_pointer[scanpos] == 'r'){
                        score += scorediff;
                        findche = true;
                        break;
                    }else if(state_pointer[scanpos] != '.'){
                        break;
                    } 
                }//for
                if(!findche){
                    for(int scanpos = 135; scanpos > 130; --scanpos){
                        if(state_pointer[scanpos] == 'r'){
                            score += scorediff;
                            findche = true;
                            break;
                        }else if(state_pointer[scanpos] != '.'){
                            break;
                        }
                    }
                }
                if(!findche){
                    for(int scanpos = 135; scanpos < 140; ++scanpos){
                        if(state_pointer[scanpos] == 'r'){
                            score += scorediff;
                            break;
                        }else if(state_pointer[scanpos] != '.'){
                            break;
                        }
                    }
                }
            }//if((src == 197 || src == 201) && dst == 167 && state_pointer[151] == 'I')
            
        
        }else if(p == 'H'){

            if(src == 164 && dst == 68 && state_pointer[52] == 'e'){
                short bonus = ::round(zu_possibility * bp -> aiaverage[version][turn][0][0]/2);
                score += bonus/2;
                if(state_pointer[53] != '.'){ //python BUG
                    score += bonus;
                }
                if(state_pointer[51] == 'd'){
                    score += bonus;
                }
            }
            
            if(src == 170 && dst == 74 && state_pointer[58] == 'e'){
                short bonus = ::round(zu_possibility * bp -> aiaverage[version][turn][0][0]/2);
                score += bonus/2;
                if(state_pointer[57] != '.'){ //python BUG
                    score += bonus;
                }
                if(state_pointer[59] == 'd'){
                    score += bonus;
                }
            }

            if((src == 164 && dst == 52 && state_pointer[52] == 'e' && (state_pointer[51] == 'd' || state_pointer[51] == 'r')) || \
                    (src == 170 && dst == 58 && state_pointer[58] == 'e' && (state_pointer[59] == 'd' || state_pointer[59] == 'r'))){
                        if((src == 164 && state_pointer[148] == 'p') || (src == 170 && state_pointer[154] == 'p')) {;}
                        if((bp -> che) < (bp -> che_opponent) || bp -> che == 0 || (bp -> score_rough < 150)) {score -= 150; }
                        //else if(che == che_opponent): Python BUG
                    }
         
        }else if(p == 'I' && state_pointer[src - 32] != 'r' && state_pointer[src - 32] != 'p'){
            if(state_pointer[src - 48] == 'i') {
                score += 25; //制约对方兵
            }
            else{
                score += 20;
            }
        }//else if I
    }//else
        
    if(q >= 'A' && q <= 'Z'){
        int k = 254 - dst;
        if(q == 'R' || q == 'N' || q == 'B' || q == 'A' || q == 'C' || q == 'P'){
            score +=  pst[intq][k];
        }
        else{
            if(q != 'U'){
                score += bp -> aiaverage[version][1 - turn][0][0];              
            }else{
                score += bp -> aiaverage[version][1 - turn][1][k];
            }
            if(q == 'D'){
                if(bp -> score_rough > 150){
                    score +=  45 * (possible_che/2 + bp -> che);
                }else{
                    score +=  30 * (possible_che/2 + bp -> che);
                }                   
            }//if(q == 'D')
        }           
    }//capture
    if(score < LOWER_BOUND) return LOWER_BOUND;
    else if(score > UPPER_BOUND) return UPPER_BOUND;
    return (short)round(score);
}

inline void complicated_kongtoupao_score_function4(board::AIBoard4* bp, short* kongtoupao_score, short* kongtoupao_score_opponent){
    if(bp -> kongtoupao > bp -> kongtoupao_opponent){
        if((bp -> che >= bp -> che_opponent && bp -> che > 0) || bp -> kongtoupao >= 3)
            *kongtoupao_score = 180;
        else
            *kongtoupao_score = 140;
    }

    else if(bp -> kongtoupao_opponent > bp -> kongtoupao){
        if((bp -> che_opponent >= bp -> che && bp -> che_opponent > 0) || bp -> kongtoupao_opponent >= 3)
            *kongtoupao_score_opponent = 180;
        else
            *kongtoupao_score_opponent = 140;
    }
}

void register_score_functions4(){
    score_bean4.insert({"complicated_score_function4", complicated_score_function4});
    kongtoupao_score_bean4.insert({"complicated_kongtoupao_score_function4", complicated_kongtoupao_score_function4});
    thinker_bean4.insert({"mtd_thinker4", mtd_thinker4});
}

std::string SearchScoreFunction4(void* score_func, int type){
    if(type == 0){
        for(auto it = score_bean4.begin(); it != score_bean4.end(); ++it){
            if(it -> second == score_func){
                return it -> first;
            }
        } 
        return "";
    }else if(type == 1){
        for(auto it = kongtoupao_score_bean4.begin(); it != kongtoupao_score_bean4.end(); ++it){
            if(it -> second == score_func){
                return it -> first;
            }
        } 
        return "";
    }else if(type == 2){
        for(auto it = thinker_bean4.begin(); it != thinker_bean4.end(); ++it){
            if(it -> second == score_func){
                return it -> first;
            }
        } 
        return "";
    }
    return "";
}


std::string mtd_thinker4(board::AIBoard4* bp){
    constexpr short MATE_UPPER = 2600;
    constexpr short EVAL_ROBUSTNESS = 15;
    bool execute = false;
    bp -> Scan();
    bool traverse_all_strategy = true;
    int max_depth = (bp -> round < 15?6:7);
    int quiesc_depth = (bp -> round < 15?1:0);
    int depth = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for(depth = 6; depth <= max_depth; ++depth){
        short lower = -MATE_UPPER, upper = MATE_UPPER;
        int me = 0, op = 0;
        while(lower < upper - EVAL_ROBUSTNESS){
            short gamma = (lower + upper + 1)/2; //不会溢出
            short score = mtd_alphabeta4(bp, gamma, depth + quiesc_depth, true, true, true, quiesc_depth, traverse_all_strategy, &me, &op);
            if(me <= depth + quiesc_depth + 2 || op <= depth + quiesc_depth + 2){
                execute = true;
                break;
            }
            if(score >= gamma) { lower = score; }
            if(score < gamma) { upper = score; }
        }
        if(!execute){
            mtd_alphabeta4(bp, lower, depth + quiesc_depth, true, true, true, quiesc_depth, traverse_all_strategy, &me, &op);
            if(me <= depth + quiesc_depth + 2 || op <= depth + quiesc_depth + 2){
                execute = true;
            }
        }
        size_t int_ms = (size_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        if(execute || int_ms > 50000 || depth == max_depth){
            bp -> Scan();
            if(me <= 6 && (bp -> covered > 0 || bp -> covered_opponent > 0)){
                short lower = -MATE_UPPER, upper = MATE_UPPER;
                while(lower < upper - EVAL_ROBUSTNESS){
                    short gamma = (lower + upper + 1)/2; //不会溢出
                    short score = calleval4(bp, gamma, {2, 3}, {true, false}, true, false);
                    if(score >= gamma) { lower = score; }
                    if(score < gamma) { upper = score; }
                }
                calleval4(bp, lower, {2, 3}, {true, false}, true, false);
            }
            auto move = (*bp -> tp_move)[{bp -> zobrist_hash, bp -> turn}];
            if(move == std::pair<unsigned char, unsigned char>({0, 0})){
                unsigned char mate_src = 0, mate_dst = 0;
                std::tuple<short, unsigned char, unsigned char> legal_moves_tmp[MAX_POSSIBLE_MOVES];
                int num_of_legal_moves_tmp = 0;
                bool killer_is_alive = false;
                short killer_score = 0;
                bp -> GenMovesWithScore<true, false>(legal_moves_tmp, num_of_legal_moves_tmp, NULL, killer_score, mate_src, mate_dst, killer_is_alive);
                std::cout << "My name: " << bp -> GetName() <<" [AM I FAILED?]" << num_of_legal_moves_tmp << " My move: " << bp -> translate_ucci(std::get<1>(legal_moves_tmp[0]), std::get<2>(legal_moves_tmp[0])) << ", duration = " << int_ms << ", depth = " << depth + quiesc_depth << "." << std::endl;
                if(num_of_legal_moves_tmp != 0){
                    return bp -> translate_ucci(std::get<1>(legal_moves_tmp[0]), std::get<2>(legal_moves_tmp[0]));
                }
            }
            std::cout << "My name: " << bp -> GetName()  << " My move: " << bp -> translate_ucci(move.first, move.second) << ", duration = " << int_ms << ", depth = " << depth + quiesc_depth << "." << std::endl;
            return bp -> translate_ucci(move.first, move.second);
        }
    }
    return "";
}

short mtd_quiescence4(board::AIBoard4* self, const short gamma, int quiesc_depth, const bool root, int* me, int* op){
    constexpr short MATE_UPPER = 2600;
    constexpr int TOPK = 3;
    unsigned char mate_src = 0, mate_dst = 0;
    *me = std::numeric_limits<int>::max()/2;
    *op = std::numeric_limits<int>::min()/2;
    std::function<short()> evaluate = [self]() -> short{
        return self -> score + self -> kongtoupao_score - self -> kongtoupao_score_opponent + self -> ScanProtectors();
    };
    std::tuple<short, unsigned char, unsigned char> legal_moves_tmp[MAX_POSSIBLE_MOVES];
    int num_of_legal_moves_tmp = 0;
    const char* _state_pointer = self -> turn? self -> state_red : self -> state_black;
    bool killer_is_alive = false;
    short killer_score = 0;
    bool mate = quiesc_depth ? self -> GenMovesWithScore<true, false>(legal_moves_tmp, num_of_legal_moves_tmp, NULL, killer_score, mate_src, mate_dst, killer_is_alive) : \
        self -> GenMovesWithScore<false, true>(legal_moves_tmp, num_of_legal_moves_tmp, NULL, killer_score, mate_src, mate_dst, killer_is_alive);
    std::pair<uint32_t, bool> pairturn = {self -> zobrist_hash, self -> turn};    
    std::pair<uint32_t, int> pair = {self -> zobrist_hash, (quiesc_depth << 1) + (int)self -> turn};
    if(mate) {
        (*self -> tp_move)[pairturn] = {mate_src, mate_dst}; 
        *me = 1; 
        *op = std::numeric_limits<int>::max()/2;
        return MATE_UPPER; 
    }
    if(self -> Executed(&mate, legal_moves_tmp, num_of_legal_moves_tmp, true)){
        *me = std::numeric_limits<int>::max()/2;
        *op = 2;
        return -MATE_UPPER;
    }
    if(quiesc_depth == 0) { 
        *op = std::numeric_limits<int>::max()/2;
        return evaluate();
    }
    std::pair<short, short> entry(-MATE_UPPER, MATE_UPPER);
    if(self -> tp_score -> find(pair) != self -> tp_score -> end()){
        entry = (*self -> tp_score)[pair];
    }
    if(entry.first >= gamma){
        *op = std::numeric_limits<int>::max()/2;
        return entry.first;
    }
    if(entry.second < gamma){
        *op = std::numeric_limits<int>::max()/2;
        return entry.second;
    }
    short score = 0, best = -MATE_UPPER; 
    std::function<bool(short, unsigned char, unsigned char, short*)> judge = [&](short score, unsigned char src, unsigned char dst, short* best){
        bool update = score > *best;
        if(update){
            *best = score;
        }
        if(*best >= gamma && update){
            if(src && dst && root){
                (*self -> tp_move)[pairturn] = {src, dst};
            }
            return true;
        }
        return false;
    };
    bool into = false;
    for(int j = 0; j < num_of_legal_moves_tmp; ++j){
        auto move_score_tuple = legal_moves_tmp[j];
        unsigned char src = std::get<1>(move_score_tuple), dst = std::get<2>(move_score_tuple);
        bool mate_oppo = self -> Ismate_After_Move(src, dst);
        if(j < TOPK || _state_pointer[dst] == 'r' || _state_pointer[dst] == 'n' || _state_pointer[dst] == 'c' || _state_pointer[dst] == 'u' ||  (_state_pointer[dst] >= 'd' && _state_pointer[dst] <= 'i') || mate || mate_oppo){//走这步可以将到对手, 或正在被对手将军
            into = true;
            bool retval = self -> Move(src, dst, std::get<0>(move_score_tuple));
            if(retval){
                int metmp = 0, optmp = 0;
                score = -mtd_quiescence4(self, 1 - gamma, quiesc_depth - 1, false, &metmp, &optmp);
                *me = std::min(*me, optmp + 1);
                *op = std::max(*op, metmp + 1);
            }
            self -> UndoMove(1);
            if(score == MATE_UPPER){
                best = MATE_UPPER;
                (*self -> tp_move)[pairturn] = {src, dst};
                break;
            }
            if(retval && judge(score, src, dst, &best)){
                break;
            }
        }
    }
    if(!into) {
        self -> Scan();
        return evaluate();
    }
    if(best >= gamma){
        (*self -> tp_score)[pair] = {best, entry.second};
    }else{
        (*self -> tp_score)[pair] = {entry.first, best};
    }
    if(*op < 0){
        *op = std::numeric_limits<int>::max()/2;
    }
    if(*me < 0){
        *me = std::numeric_limits<int>::max()/2;
    }
    return best;
}

short mtd_alphabeta4(board::AIBoard4* self, const short gamma, int depth, const bool root, const bool nullmove, const bool nullmove_now, const int quiesc_depth, const bool traverse_all_strategy, int* me, int* op){
    constexpr short MATE_UPPER = 2600;
    unsigned char mate_src = 0, mate_dst = 0;
    *me = std::numeric_limits<int>::max()/2;
    *op = std::numeric_limits<int>::min()/2;
    if(root) {
        self -> Scan();     
        self -> original_depth = depth;
    }
    if(!root && self -> hist -> find(self -> state_red) != self -> hist -> end()){
        //假设AI执红。校验对象红方(self->original_turn)
        //红方走了一步, 形成局面A, self -> hist[self -> state_red] == false(因为现在是黑方)
        //如果和当前局面重复(当前局面为!self -> original_turn), 且turn也相同, 直接判断黑方胜利
        *me = 0;
        *op = std::numeric_limits<int>::max()/2;
        return MATE_UPPER;
    }
    std::tuple<short, unsigned char, unsigned char> legal_moves_tmp[MAX_POSSIBLE_MOVES];
    int num_of_legal_moves_tmp = 0;
    depth = std::max(depth, quiesc_depth);
    std::pair<unsigned char, unsigned char> killer = {0, 0};
    bool killer_is_alive = false;
    short killer_score = 0;
    std::pair<uint32_t, bool> pairturn = {self -> zobrist_hash, self -> turn};  
    std::pair<uint32_t, int> pair = {self -> zobrist_hash, (depth << 1) + (int)self -> turn};
    if(self -> tp_move -> find(pairturn) != self -> tp_move -> end()){
        killer = (*self -> tp_move)[pairturn];
        killer_is_alive = true;
    }
    bool mate = (depth == quiesc_depth ? self -> GenMovesWithScore<false, true>(legal_moves_tmp, num_of_legal_moves_tmp, killer_is_alive?&killer:NULL, killer_score, mate_src, mate_dst, killer_is_alive) : \
        self -> GenMovesWithScore<true, false>(legal_moves_tmp, num_of_legal_moves_tmp, killer_is_alive?&killer:NULL, killer_score, mate_src, mate_dst, killer_is_alive));
    if(mate) { 
        (*self -> tp_move)[pairturn] = {mate_src, mate_dst};
        *me = 1; 
        *op = std::numeric_limits<int>::max()/2;
        return MATE_UPPER;
    }
    if(self -> Executed(&mate, legal_moves_tmp, num_of_legal_moves_tmp, true)){
        *me = std::numeric_limits<int>::max()/2;
        *op = 2;
        return -MATE_UPPER;
    }
    if(depth == quiesc_depth){
        short score = mtd_quiescence4(self, gamma, quiesc_depth, true, me, op);
        return score;
    }
    std::pair<short, short> entry(-MATE_UPPER, MATE_UPPER);
    if(self -> tp_score -> find(pair) != self -> tp_score -> end()){
        entry = (*self -> tp_score)[pair];
    }
    if(entry.first >= gamma && (!root || killer_is_alive)){
        *op = std::numeric_limits<int>::max()/2;
        return entry.first;
    }
    if(entry.second < gamma){
        *op = std::numeric_limits<int>::max()/2;
        return entry.second;
    }
    short score = 0, best = -MATE_UPPER;
    std::function<bool(short, unsigned char, unsigned char, short*)> judge = [&](short score, unsigned char src, unsigned char dst, short* best){
        bool update = score > *best;
        if(update){
            *best = score;
        }
        if(*best >= gamma && update){
            if(src && dst){
                (*self -> tp_move)[pairturn] = {src, dst};
            }
            return true;
        }
        return false;
    };
    do{
        if(nullmove && nullmove_now && depth > 3 && !mate && !root){
            self -> NULLMove();
            int metmp = 0, optmp = 0;  
            score = -mtd_alphabeta4(self, 1 - gamma, depth - 3, false, nullmove, nullmove, quiesc_depth, traverse_all_strategy, &metmp, &optmp); //Attempt: false --> nullmove
            *me = std::min(optmp + 1, *me);
            *op = std::max(metmp + 1, *op);
            self -> UndoMove(0);
            if(judge(score, 0, 0, &best) && (!root || !traverse_all_strategy)){
                break;
            }
        }

        if(killer_is_alive){
            bool retval = self -> Move(killer.first, killer.second, killer_score);
            int metmp = 0, optmp = 0; 
            if(retval){ 
                score = -mtd_alphabeta4(self, 1 - gamma, depth - 1, false, nullmove, nullmove, quiesc_depth, traverse_all_strategy, &metmp, &optmp);
                *me = std::min(optmp + 1, *me);
                *op = std::max(metmp + 1, *op);
            }
            self -> UndoMove(1);
            if(score == MATE_UPPER){
                best = MATE_UPPER;
                break;
            }
            if(retval && judge(score, killer.first, killer.second, &best) && (!root || !traverse_all_strategy)){
                break;
            }
        }
        for(int j = 0; j < num_of_legal_moves_tmp; ++j){
            auto move_score_tuple = legal_moves_tmp[j];
            auto src = std::get<1>(move_score_tuple), dst = std::get<2>(move_score_tuple);
            if(killer_is_alive && src == killer.first && dst == killer.second){
                continue;
            }
            bool retval = self -> Move(src, dst, std::get<0>(move_score_tuple));
            int metmp = 0, optmp = 0; 
            if(retval){
                score = -mtd_alphabeta4(self, 1 - gamma, depth - 1, false, nullmove, nullmove, quiesc_depth, traverse_all_strategy, &metmp, &optmp);
                *me = std::min(optmp + 1, *me);
                *op = std::max(metmp + 1, *op);
            }
            self -> UndoMove(1);
            if(score == MATE_UPPER){
                best = MATE_UPPER;
                (*self -> tp_move)[pairturn] = {src, dst};
                break;
            }
            if(retval && judge(score, src, dst, &best) && (!root || !traverse_all_strategy)){
                break;
            }
        }
    }while(false);
    if(best >= gamma){
        (*self -> tp_score)[pair] = {best, entry.second};
    }else{
        (*self -> tp_score)[pair] = {entry.first, best};
    }
    if(*op < 0){
        *op = std::numeric_limits<int>::max()/2;
    }
    if(*me < 0){
        *me = std::numeric_limits<int>::max()/2;
    }
    return best;
}


short mtd_alphabeta_doublerecursive4(board::AIBoard4* self, const int ver, const short gamma, std::vector<int>& depths, std::vector<bool>& traverse_all_strategies, const bool root, const bool nullmove, \
    const bool nullmove_now, const bool pruning, const float discount_factor, std::unordered_map<unsigned char, char>& uncertainty_dict, bool* need_clamp){
    constexpr short MATE_UPPER = 2600;
    *need_clamp = false;
    auto src_move_is_from_uncertainty_dict = [&](unsigned char i){
        for(auto j = uncertainty_dict.begin(); j != uncertainty_dict.end(); ++j){
            if(j -> first == i && self -> turn == self -> original_turns[ver]){
                return true;
            }
            if(j -> first == 254 - i && self -> turn != self -> original_turns[ver]){
                return true;
            }
        }
        return false;
    };

    self -> version = ver;
    int depth = depths[ver];
    bool traverse_all_strategy = traverse_all_strategies[ver];
    unsigned char mate_src = 0, mate_dst = 0;
    if(root) {
        self -> Scan();  
    }
    if(!root && self -> hist -> find(self -> state_red) != self -> hist -> end()){
        //假设AI执红。校验对象红方(self->original_turn)
        //红方走了一步, 形成局面A, self -> hist[self -> state_red] == false(因为现在是黑方)
        //如果和当前局面重复(当前局面为!self -> original_turn), 且turn也相同, 直接判断黑方胜利
        return MATE_UPPER;
    }
    std::tuple<short, unsigned char, unsigned char> legal_moves_tmp[MAX_POSSIBLE_MOVES];
    int num_of_legal_moves_tmp = 0;
    depth = std::max(depth, 0);
    std::pair<unsigned char, unsigned char> killer = {0, 0};
    bool killer_is_alive = false;
    short killer_score = 0;
    if(self -> tp_move -> find({self -> zobrist_hash, self -> turn}) != self -> tp_move -> end()){
        killer = (*self -> tp_move)[{self -> zobrist_hash, self -> turn}];
        killer_is_alive = true;
    }
    bool mate = (depth ? self -> GenMovesWithScore<true, false>(legal_moves_tmp, num_of_legal_moves_tmp, killer_is_alive?&killer:NULL, killer_score, mate_src, mate_dst, killer_is_alive) : \
        self -> GenMovesWithScore<false, true>(legal_moves_tmp, num_of_legal_moves_tmp, killer_is_alive?&killer:NULL, killer_score, mate_src, mate_dst, killer_is_alive));
    if(mate) { 
        *need_clamp = src_move_is_from_uncertainty_dict(mate_src);
        (*self -> tp_move)[{self -> zobrist_hash, self -> turn}] = {mate_src, mate_dst}; 
        return MATE_UPPER; 
    }
    if((size_t)ver == depths.size() - 1 && self -> Executed(&mate, legal_moves_tmp, num_of_legal_moves_tmp, true)){
        return -MATE_UPPER;
    }
    if(depth == 0){
        return eval4(self, ver+1, gamma, depths, traverse_all_strategies, nullmove, nullmove_now, pruning, discount_factor);
    }
    std::pair<short, short> entry(-MATE_UPPER, MATE_UPPER);
    std::pair<uint32_t, int> pair = {self -> zobrist_hash, (depth << 1) + (int)self -> turn};
    if(self -> tp_score -> find(pair) != self -> tp_score -> end()){
        entry = (*self -> tp_score)[pair];
    }
    if(entry.first >= gamma && (!root || killer_is_alive)){
        return entry.first;
    }
    if(entry.second < gamma){
        return entry.second;
    }
    short score = 0, best = -MATE_UPPER;
    std::function<bool(short, unsigned char, unsigned char, short*)> judge = [&](short score, unsigned char src, unsigned char dst, short* best){
        bool update = score > *best;
        if(update){
            *best = score;
        }
        if(*best >= gamma && update){
            if(src && dst){
                (*self -> tp_move)[{self -> zobrist_hash, self -> turn}] = {src, dst};
                *need_clamp =  src_move_is_from_uncertainty_dict(src);
            }
            return true;
        }
        return false;
    };
    do{
        if(nullmove && nullmove_now && depth > 3 && !mate && !root){
            depths[ver] -= 3;
            self -> NULLMove();
            score = -mtd_alphabeta_doublerecursive4(self, ver, 1 - gamma, depths, traverse_all_strategies, false, nullmove, nullmove, pruning, discount_factor, uncertainty_dict, need_clamp); //Attempt: false --> nullmove
            self -> UndoMove(0);
            depths[ver] += 3;
            if(judge(score, 0, 0, &best) && (!root || !traverse_all_strategy)){
                break;
            }
        }
        if(killer_is_alive){
            bool retval = self -> Move(killer.first, killer.second, killer_score);
            if(retval){
                depths[ver] -= 1;
                score = -mtd_alphabeta_doublerecursive4(self, ver, 1 - gamma, depths, traverse_all_strategies, false, nullmove, nullmove, pruning, discount_factor, uncertainty_dict, need_clamp);
                depths[ver] += 1;
            }
            self -> UndoMove(1);
            if(retval && judge(score, killer.first, killer.second, &best) && (!root || !traverse_all_strategy)){
                break;
            }
        }

        for(int j = 0; j < num_of_legal_moves_tmp; ++j){
            auto move_score_tuple = legal_moves_tmp[j];
            auto src = std::get<1>(move_score_tuple), dst = std::get<2>(move_score_tuple);
            bool retval = self -> Move(src, dst, std::get<0>(move_score_tuple));
            if(retval){
                depths[ver] -= 1;
                score = -mtd_alphabeta_doublerecursive4(self, ver, 1 - gamma, depths, traverse_all_strategies, false, nullmove, nullmove, pruning, discount_factor, uncertainty_dict, need_clamp);
                depths[ver] += 1;
            }
            self -> UndoMove(1);
            if(retval && judge(score, src, dst, &best) && (!root || !traverse_all_strategy)){
                break;
            }
        }
    }while(false);
    if(best >= gamma){
        (*self -> tp_score)[pair] = {best, entry.second};
    }else{
        (*self -> tp_score)[pair] = {entry.first, best};
    }
    return best;
}

void _inner_recur(board::AIBoard4* self, const int ver, std::unordered_map<unsigned char, char>& uncertainty_dict, std::vector<unsigned char>& uncertainty_keys, \
    std::unordered_map<std::pair<int, int>, short, myhash<int, int>>& result_dict, std::unordered_map<std::pair<int, int>, short, myhash<int, int>>& counter_dict, const int index, const int me, const int op, const bool pruning, const short score, const short gamma, \
    std::vector<int>& depths, std::vector<bool>& traverse_all_strategies, const bool nullmove, const bool nullmove_now, const float discount_factor){
    const int THRES = 300;
    bool need_clamp = false;
    if(index == 0 && uncertainty_keys.empty()){
        result_dict[{1, 1}] += mtd_alphabeta_doublerecursive4(self, ver, gamma, depths, traverse_all_strategies, true, nullmove, nullmove_now, pruning, discount_factor, uncertainty_dict, &need_clamp);
        counter_dict[{1, 1}] += 1;
        return;
    }
    if((size_t)index >= uncertainty_keys.size()){
        self -> score = score;
        self -> CalcVersion(ver, discount_factor);
        short res =  mtd_alphabeta_doublerecursive4(self, ver, gamma, depths, traverse_all_strategies, true, nullmove, nullmove_now, pruning, discount_factor, uncertainty_dict, &need_clamp);
        result_dict[{me, op}] += ((need_clamp && res >= THRES) ? THRES : res);
        counter_dict[{me, op}] += 1;
    }else{
        bool turn = self -> turn, notturn = !self -> turn;
        char* state_pointer = turn ? self -> state_red : self -> state_black;
        char* state_pointer_oppo = notturn ? self -> state_red : self -> state_black;
        unsigned char key = uncertainty_keys[index];
        char c = uncertainty_dict[key];
        switch(c){
            case 'U': {
                for(char c : MINGZI){
                    int intchar = turn ? (int)c : ((int)c) ^ 32;
                    if(self -> aidi[ver][turn][intchar] > 0){
                        --self -> aidi[ver][turn][intchar];
                        uint32_t zobrist_before = self -> zobrist_hash;
                        int zobrist_key = turn ? key : 254 - key;
                        self -> zobrist_hash ^= self -> zobrist[(int)self -> state_red[zobrist_key]][zobrist_key];
                        state_pointer[key] = c;
                        state_pointer_oppo[254 - key] = self -> swapcase(c);
                        self -> zobrist_hash ^= self -> zobrist[(int)self -> state_red[zobrist_key]][zobrist_key];
                        short score_diff = pst[(int)c][key] - self -> aiaverage[ver-1][turn][1][key];
                        _inner_recur(self, ver, uncertainty_dict, uncertainty_keys, result_dict, counter_dict, index+1, me*(self -> aidi[ver][turn][intchar] + 1), op, pruning, score + score_diff/2, gamma, depths, \
                            traverse_all_strategies, nullmove, nullmove_now, discount_factor);
                        state_pointer[key] = 'U';
                        state_pointer_oppo[254 - key] = 'u';
                        self -> zobrist_hash = zobrist_before;
                        ++self -> aidi[ver][turn][intchar];
                    }
                }
                break;
            }

            case 'u': {
                for(char c: MINGZI){
                    int intchar = notturn ? (int)c : ((int)c) ^ 32;
                    if(self -> aidi[ver][notturn][intchar] > 0){
                        --self -> aidi[ver][notturn][intchar];
                        uint32_t zobrist_before = self -> zobrist_hash;
                        int zobrist_key = turn ? key : 254 - key;
                        self -> zobrist_hash ^= self -> zobrist[(int)self -> state_red[zobrist_key]][zobrist_key];
                        state_pointer[key] = self -> swapcase(c);
                        state_pointer_oppo[254 - key] = c;
                        self -> zobrist_hash ^= self -> zobrist[(int)self -> state_red[zobrist_key]][zobrist_key];
                        short score_diff = pst[(int)c][254 - key] - self -> aiaverage[ver-1][notturn][1][254 - key];
                        _inner_recur(self, ver, uncertainty_dict, uncertainty_keys, result_dict, counter_dict, index+1, me, op*(self -> aidi[ver][notturn][intchar] + 1), pruning, score-score_diff/2, gamma, depths, \
                            traverse_all_strategies, nullmove, nullmove_now, discount_factor);
                        state_pointer[key] = 'u';
                        state_pointer_oppo[254 - key] = 'U';
                        self -> zobrist_hash = zobrist_before;
                        ++self -> aidi[ver][notturn][intchar];
                    }
                }
                break;
            }
        }//逐步展开暗子
    }
}


short eval4(board::AIBoard4* self, const int ver, const short gamma, std::vector<int>& depths, std::vector<bool>& traverse_all_strategies, const bool nullmove, const bool nullmove_now, const bool pruning, \
    const float discount_factor){
    self -> original_turns[ver] = self -> turn;
    std::unordered_map<unsigned char, char> uncertainty_dict;
    std::vector<unsigned char> uncertainty_keys;
    std::unordered_map<std::pair<int, int>, short, myhash<int, int>> result_dict;
    std::unordered_map<std::pair<int, int>, short, myhash<int, int>> counter_dict;
    const char* state_pointer = self -> turn ? self -> state_red : self -> state_black;
    bool need_clamp = false;
    std::unordered_map<unsigned char, char> empty_map;
    if(ver == 0){
        return mtd_alphabeta_doublerecursive4(self, 0, gamma, depths, traverse_all_strategies, true, nullmove, nullmove_now, pruning, discount_factor, empty_map, &need_clamp);
    }

    else if((size_t)ver >= depths.size()){
        int me = 0, op = 0;
        return mtd_quiescence4(self, gamma, 0, true, &me, &op);
    }

    else{
        memcpy(self -> aidi[ver], self -> aidi[ver-1], sizeof(self -> aidi[ver]));
        for(unsigned char i = 51; i <= 203; ++i){
            if((i & 15) < 3 || (i & 15) > 11) { continue; }
            if(state_pointer[i] == 'U' || state_pointer[i] == 'u'){
                uncertainty_dict[i] = state_pointer[i];
                uncertainty_keys.push_back(i);
            }
        }
        short nowscore = self -> score;
        _inner_recur(self, ver, uncertainty_dict, uncertainty_keys, result_dict, counter_dict, 0, 1, 1, pruning, self -> score, gamma, depths, traverse_all_strategies, nullmove, nullmove_now, discount_factor);
        self -> score = nowscore;
        int nu = 0, de = 0; //numerator, denominator;
        for(auto it = result_dict.begin(); it != result_dict.end(); ++it){
            auto& item = it -> first;
            auto combinations = item.first * item.second;
            nu += (it -> second) * combinations;
            de += counter_dict[item] * combinations;
        }
        return self -> div(nu, de);
    }
    return 0;
}

short calleval4(board::AIBoard4* self, const short gamma, std::vector<int> depths, std::vector<bool> traverse_all_strategies, const bool nullmove, const bool pruning){
    memset(self -> original_turns, self -> turn, sizeof(self -> original_turns));
    return eval4(self, 0, gamma, depths, traverse_all_strategies, nullmove, nullmove, pruning, self -> discount_factor);
}
