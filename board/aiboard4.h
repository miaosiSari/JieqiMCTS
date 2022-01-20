/*
* Created by Si Miao 2021/07/31
* Copyright (c) 2021. All rights reserved.
* Last modified 2021/07/31
*/
#ifndef aiboard4_h
#define aiboard4_h
#define MAX 257
#define CHESS_BOARD_SIZE 256
#define MAX_POSSIBLE_MOVES 120
#define A0 195 //(0, 0)坐标
#define I0 203 //(0, 8)坐标
#define A9 51 //(9, 0)坐标
#define I9 59 //(9, 8)坐标
#define NORTH -16
#define EAST 1
#define SOUTH 16
#define WEST -1
#define GS(x) std::get<0>(x)
extern unsigned char L1[256][256];
#include <cstddef>
#include <vector>
#include <string>
#include <tuple>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <functional>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <random>
#include <chrono>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <stack>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <functional>
#include "../global/global.h"
#include "thinker.h"
#define ROOTED 0
#define CLEAR_EVERY_DEPTH false
#define CH(X) self->C(X)

extern std::unordered_map<int, std::unordered_map<std::pair<uint32_t, bool>, std::pair<unsigned char, unsigned char>, myhash<uint32_t, bool>>> tp_move_bean;
extern std::unordered_map<int, std::unordered_map<std::pair<uint32_t, int>, std::pair<short, short>, myhash<uint32_t, int>>> tp_score_bean;
extern char** _dir;
extern const std::string MINGZI;

namespace board{
    class AIBoard4;
}

typedef short(*SCORE4)(board::AIBoard4* bp, const char* state_pointer, unsigned char src, unsigned char dst);
typedef void(*KONGTOUPAO_SCORE4)(board::AIBoard4* bp, short* kongtoupao_score, short* kongtoupao_score_opponent);
typedef std::string(*THINKER4)(board::AIBoard4* bp);
void register_score_functions4();
std::string SearchScoreFunction4(void* score_func, int type);
extern short pst[91][204];
template <typename K, typename V>
extern V GetWithDefUnordered(const std::unordered_map<K,V>& m, const K& key, const V& defval);


namespace board{
class AIBoard4 : public Thinker{
public:
    short aiaverage[VERSION_MAX][2][2][256];
    unsigned char aisumall[VERSION_MAX][2];
    unsigned char aidi[VERSION_MAX][2][123];
    int version = 0;
    int round = 0;
    bool turn = true; //true红black黑
    const bool original_turn;//游戏时的红黑
    bool original_turns[VERSION_MAX];
    int original_depth;
    unsigned char protector = 4;
    unsigned char protector_oppo = 4;
    unsigned char all = 0;
    unsigned char che = 0;
    unsigned char che_opponent = 0;
    unsigned char zu = 0;
    unsigned char covered = 0;
    unsigned char covered_opponent = 0;
    unsigned char endline = 0;
    short score_rough = 0;
    unsigned char kongtoupao = 0;
    unsigned char kongtoupao_opponent = 0;
    short kongtoupao_score = 0;
    short kongtoupao_score_opponent = 0;
    uint32_t zobrist_hash = 0;
    const float discount_factor;
    char state_red[MAX];
    char state_black[MAX];
    std::stack<std::tuple<unsigned char, unsigned char, char>> cache;
    short score;//局面分数
    std::stack<short> score_cache;
    std::unordered_set<uint32_t> zobrist_cache;
    std::set<unsigned char> rooted_chesses;
    //tp_move: (zobrist_key, turn) --> move
    std::unordered_map<std::pair<uint32_t, bool>, std::pair<unsigned char, unsigned char>, myhash<uint32_t, bool>>* tp_move;
    //tp_score: (zobrist_key, turn, depth <depth * 2 + turn>) --> (lower, upper)
    std::unordered_map<std::pair<uint32_t, int>, std::pair<short, short>, myhash<uint32_t, int>>* tp_score;
    std::unordered_map<std::string, bool>* hist;
    AIBoard4()=delete;
    AIBoard4(const char another_state[MAX], bool turn, int round, const unsigned char di[2][123], short score, std::unordered_map<std::string, bool>* hist,
        std::unordered_map<std::pair<uint32_t, bool>, std::pair<unsigned char, unsigned char>, myhash<uint32_t, bool>>* tp_move_bean,
        std::unordered_map<std::pair<uint32_t, int>, std::pair<short, short>, myhash<uint32_t, int>>* tp_score_bean) noexcept;
    AIBoard4(const AIBoard4& another_board) = delete;
    virtual ~AIBoard4()=default;
    void Reset() noexcept;
    void SetScoreFunction(std::string function_name, int type);
    std::string SearchScoreFunction(int type);
    std::string GetName(){
        return _myname;
    }
    bool Move(const unsigned char encode_from, const unsigned char encode_to, short score_step);
    void NULLMove();
    void UndoMove(int type);
    short ScanProtectors();
    void Scan();
    void KongTouPao(const char* _state_pointer, int pos, bool t);
    template<bool needscore, bool return_after_mate> 
    bool GenMovesWithScore(std::tuple<short, unsigned char, unsigned char> legal_moves[MAX_POSSIBLE_MOVES], int& num_of_legal_moves, std::pair<unsigned char, unsigned char>* killer, short& killer_score, unsigned char& mate_src, unsigned char& mate_dst, bool& killer_is_alive);
    template<bool doublereverse> bool Mate();
    bool Executed(bool* oppo_mate, std::tuple<short, unsigned char, unsigned char> legal_moves_tmp[], int num_of_legal_moves_tmp, bool calc);
    bool ExecutedDebugger(bool *oppo_mate);
    bool Ismate_After_Move(unsigned char src, unsigned char dst);
    void CalcVersion(const int ver, const float discount_factor);
    void CopyData(const unsigned char di[2][123]);
    virtual std::string Think();
    void PrintPos(bool turn) const;
    std::string DebugPrintPos(bool turn) const;
    void print_raw_board(const char* board, const char* hint);
    template<typename... Args> void print_raw_board(const char* board, const char* hint, Args... args);
    uint32_t zobrist[123][256];
    std::function<int(int)> translate_x = [](const int x) -> int {return 12 - x;};
    std::function<int(int)> translate_y = [](const int y) -> int {return 3 + y;};
    std::function<int(int, int)> translate_x_y = [](const int x, const int y) -> int{return 195 - 16 * x + y;};
    std::function<int(int, int)> encode = [](const int x, const int y) -> int {return 16 * x + y;};  
    std::function<int(int)> reverse = [](const int x) -> int {return 254 - x;};
    std::function<char(char)> swapcase = [](const char c) -> char{
       if(isalpha(c)) {
           return c ^ 32;
       }
       return c;
    };

    std::function<void(char*)> rotate = [this](char* p){
       std::reverse(p, p+255);
       std::transform(p, p+255, p, this -> swapcase);
       p[255] = ' ';
       memset(p + 256, 0, (MAX - 256) * sizeof(char));
    };
   
    std::function<const char*(void)> getstatepointer = [this](){
	   const char* _state_pointer = (this -> turn? this -> state_red : this -> state_black);
       return _state_pointer;
    };

    std::function<unsigned char(std::string)> f = [](std::string s) -> unsigned char {
        if(s.size() != 2) return 0;
        unsigned char x = s[1] - '0';
        unsigned char y = s[0] - 'a';
        return 195 - 16 * x + y;
    };

    char operator[](std::string s){
        return state_red[f(s)];
    }

    std::function<std::string(unsigned char)> translate_single = [](unsigned char i) -> std::string{
       int x1 = 12 - (i >> 4);
       int y1 = (i & 15) - 3;
       std::string ret = "  ";
       ret[0] = 'a' + y1;
       ret[1] = '0' + x1;
       return ret;
    };

    std::function<std::string(unsigned char, unsigned char)> translate_ucci = [this](unsigned char src, unsigned char dst) -> std::string{
       return translate_single(src) + translate_single(dst);
    };

    std::function<std::string(std::tuple<short, unsigned char, unsigned char>)> translate_tuple = \
       [this](std::tuple<short, unsigned char, unsigned char> t) -> std::string{ 
       return translate_single(std::get<1>(t)) + translate_single(std::get<2>(t));
    };

    std::function<uint32_t(void)> randU32 = []() -> uint32_t{
	   //BUG: 在Windows上每次生成同样的随机数
       std::mt19937 gen(std::random_device{}());
       uint32_t randomNumber = gen();
       return randomNumber;
    };

    template<typename T>
    inline T div(T x, T y){
        if(std::is_floating_point<T>::value){
            return fabs(y) < 1e-7 ? (x/y) : 0.0;
        }
        return y != 0 ? (x / y) : 0;
    }
   
private:
    std::string _myname;
    bool _has_initialized = false;
    static const int _chess_board_size;
    static const char _initial_state[MAX];
    static const std::unordered_map<std::string, std::string> _uni_pieces;
    SCORE4 _score_func = NULL;
    KONGTOUPAO_SCORE4 _kongtoupao_score_func = NULL;
    THINKER4 _thinker_func = NULL;
    std::function<std::string(const char)> _getstring = [](const char c) -> std::string {
        std::string ret;
        const std::string c_string(1, c);
        ret = GetWithDefUnordered<std::string, std::string>(_uni_pieces, c_string, c_string);
        return ret;
    };
    std::function<std::string(int, int, bool)> _getstringxy = [this](int x, int y, bool turn) -> std::string {
        std::string ret =  turn?_getstring(state_red[encode(x, y)]):_getstring(state_black[encode(x, y)]);
        return ret;
    };
    std::function<void(void)> _initialize_zobrist = [this](){
        for(int i = 0; i < 123; ++i){
            for(int j = 0; j < 256; ++j){
                if(i != '.'){
                    zobrist[i][j] = randU32();
				}
                else
                    zobrist[i][j] = 0;
            }
        }
        for(int j = 51; j <= 203; ++j){
            if(::isalpha(state_red[j])){
                zobrist_hash ^= zobrist[(int)state_red[j]][j];
            }
        }
    };
};
}

std::string mtd_thinker4(board::AIBoard4* bp);
void complicated_kongtoupao_score_function4(board::AIBoard4* bp, short* kongtoupao_score, short* kongtoupao_score_opponent);
short complicated_score_function4(board::AIBoard4* self, const char* state_pointer, unsigned char src, unsigned char dst);
short mtd_quiescence4(board::AIBoard4* self, const short gamma, int quiesc_depth, const bool root, int* me, int* op);
short mtd_alphabeta4(board::AIBoard4* self, const short gamma, int depth, const bool root, const bool nullmove, const bool nullmove_now, const int quiesc_depth, const bool traverse_all_strategy, int* me, int* op);
short mtd_alphabeta_doublerecursive4(board::AIBoard4* self, const int ver, const short gamma, std::vector<int>& depths, std::vector<bool>& traverse_all_strategies, const bool root, const bool nullmove, \
    const bool nullmove_now, const bool pruning, const float discount_factor, std::unordered_map<unsigned char, char>& uncertainty_dict, bool* need_clamp);
void _inner_recur(board::AIBoard4* self, const int ver, std::unordered_map<unsigned char, char>& uncertainty_dict, std::vector<unsigned char>& uncertainty_keys, \
    std::unordered_map<std::pair<int, int>, unsigned char, myhash<int, int>>& result_dict, const int index, const int me, const int op, const bool pruning, const short score, const short gamma, \
    std::vector<int>& depths, std::vector<bool>& traverse_all_strategies, const bool nullmove, const bool nullmove_now, const float discount_factor);
short eval4(board::AIBoard4* self, const int ver, const short gamma, std::vector<int>& depths, std::vector<bool>& traverse_all_strategies, const bool nullmove, const bool nullmove_now, const bool pruning, \
    const float discount_factor);
short calleval4(board::AIBoard4* self, const short gamma, std::vector<int> depths, std::vector<bool> traverse_all_strategies, const bool nullmove, const bool pruning);
#endif
