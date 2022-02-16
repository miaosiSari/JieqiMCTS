#ifndef god_h
#define god_h

#include <pybind11/pybind11.h>
#include <memory>
#include <vector>
#include <string.h>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <sstream>
#include "board.h"
#include "thinker.h"
#include "aiboard4.h"
#include "../global/global.h"

namespace py = pybind11;
#define MAX_ROUNDS 200
#define NEWRED new board::AIBoard4(board_pointer -> state_red, board_pointer -> turn, board_pointer -> round, board_pointer -> di_red, tptable, pst, L1, _dir)
#define NEWBLACK new board::AIBoard4(board_pointer -> state_black, board_pointer -> turn, board_pointer -> round, board_pointer -> di_black, tptable, pst, L1, _dir)
#define InfoDict2pydict(X, Y) \
X["islegal"] = Y -> islegal; \
X["win"] = Y -> win; \
X["eat"] = Y -> eat; \
X["eat_type"] = Y -> eat_type; \
X["eat_check"] = Y -> eat_check; 
#define DILIST(X, Y) \
X.append(Y[1][(int)'R']); \
X.append(Y[1][(int)'N']); \
X.append(Y[1][(int)'B']); \
X.append(Y[1][(int)'A']); \
X.append(Y[1][(int)'C']); \
X.append(Y[1][(int)'P']); \
X.append(Y[0][(int)'r']); \
X.append(Y[0][(int)'n']); \
X.append(Y[0][(int)'b']); \
X.append(Y[0][(int)'a']); \
X.append(Y[0][(int)'c']); \
X.append(Y[0][(int)'p']); \


struct God{
    const std::string MINGZI;
    std::vector<std::tuple<char, int, int, char>> red_eat_black;
    std::vector<std::tuple<char, int, int, char>> black_eat_red;
    std::unordered_map<std::pair<uint32_t, bool>, std::pair<unsigned char, unsigned char>, myhash<uint32_t, bool>> tp_move_bean;
    std::unordered_map<std::pair<uint32_t, int>, std::pair<short, short>, myhash<uint32_t, int>> tp_score_bean;
    std::unordered_set<std::string> hist_cache;
    board::Board* board_pointer;
    std::unique_ptr<board::Thinker> ai;
    tp* tptable;
    short** pst;
    unsigned char** L1;
    char** _dir;
    God();
    ~God();
    bool Initialize(bool random, bool turn, std::string board);
    bool PrintPos();
    bool GetTurn();
    py::dict GetMeta();
    py::tuple GetState();
    inline std::shared_ptr<InfoDict> InnerMove(std::string s);
    inline std::shared_ptr<InfoDict> InnerMove(const int encode_from, const int encode_to);
    py::dict _Move(std::shared_ptr<InfoDict> p);
    py::dict Move(std::string s);
    py::dict Move(const int encode_from, const int encode_to);
    void UndoMove(int k);
    py::list GenMoves();
    py::tuple AIHint(int depth);
    std::string Hint(int depth);
    std::string translate_single(int i){
        int x1 = 12 - (i >> 4);
        int y1 = (i & 15) - 3;
        std::string ret = "  ";
        ret[0] = 'a' + y1;
        ret[1] = '0' + x1;
        return ret;
    }

    std::string translate_ucci(int src, int dst){
        return translate_single(src) + translate_single(dst);
    }

    std::function<std::string(const char)> getstring = [](const char c) -> std::string {
        std::string ret;
        const std::string c_string(1, c);
        ret = GetWithDefUnordered<std::string, std::string>(board::Board::uni_pieces, c_string, c_string);
        return ret;
    };

    std::function<bool(std::string)> check_legal = [](std::string s){
        if(s.size() != 4) return false;
        if(!(s[0] >= 'a' && s[0] <= 'i')) return false;
        if(!(s[1] >= '0' && s[1] <= '9')) return false;    
        if(!(s[2] >= 'a' && s[2] <= 'i')) return false;
        if(!(s[3] >= '0' && s[3] <= '9')) return false;  
        return true;
    };

    std::function<std::string(const char, bool, bool)> isdot = [this](const char c, bool isdark, bool turn) -> std::string {
        if(c == '.') return "";
        std::string ret = getstring(turn? ::toupper(c) : ::tolower(c));
        if(isdark){
            ret = ret + "(暗) ";
        }else{
            ret = ret + " ";
        }
        return ret;
    };

private:
    void initialize_pst(){
        pst = new short*[91];
        for(int i = 0; i < 91; ++i){
            pst[i] = new short[204];
            memset(pst[i], 0, 204*sizeof(short));
        }
        pst[65][51] = 88;
        pst[65][52] = 103;
        pst[65][53] = 92;
        pst[65][54] = 115;
        pst[65][55] = 100;
        pst[65][56] = 115;
        pst[65][57] = 92;
        pst[65][58] = 103;
        pst[65][59] = 88;
        pst[65][67] = 88;
        pst[65][68] = 88;
        pst[65][69] = 99;
        pst[65][70] = 109;
        pst[65][71] = 106;
        pst[65][72] = 109;
        pst[65][73] = 99;
        pst[65][74] = 88;
        pst[65][75] = 88;
        pst[65][83] = 85;
        pst[65][84] = 109;
        pst[65][85] = 88;
        pst[65][86] = 91;
        pst[65][87] = 88;
        pst[65][88] = 91;
        pst[65][89] = 88;
        pst[65][90] = 109;
        pst[65][91] = 85;
        pst[65][99] = 83;
        pst[65][100] = 85;
        pst[65][101] = 88;
        pst[65][102] = 86;
        pst[65][103] = 88;
        pst[65][104] = 86;
        pst[65][105] = 88;
        pst[65][106] = 85;
        pst[65][107] = 83;
        pst[65][115] = 81;
        pst[65][116] = 82;
        pst[65][117] = 85;
        pst[65][118] = 82;
        pst[65][119] = 85;
        pst[65][120] = 82;
        pst[65][121] = 85;
        pst[65][122] = 82;
        pst[65][123] = 81;
        pst[65][131] = 73;
        pst[65][132] = 85;
        pst[65][133] = 75;
        pst[65][134] = 85;
        pst[65][135] = 75;
        pst[65][136] = 85;
        pst[65][137] = 75;
        pst[65][138] = 85;
        pst[65][139] = 73;
        pst[65][147] = 82;
        pst[65][148] = 69;
        pst[65][149] = 89;
        pst[65][150] = 69;
        pst[65][151] = 93;
        pst[65][152] = 69;
        pst[65][153] = 89;
        pst[65][154] = 69;
        pst[65][155] = 82;
        pst[65][163] = 69;
        pst[65][164] = 90;
        pst[65][165] = 65;
        pst[65][166] = 100;
        pst[65][167] = 61;
        pst[65][168] = 100;
        pst[65][169] = 65;
        pst[65][170] = 90;
        pst[65][171] = 69;
        pst[65][179] = 65;
        pst[65][180] = 57;
        pst[65][181] = 95;
        pst[65][182] = 69;
        pst[65][183] = 105;
        pst[65][184] = 69;
        pst[65][185] = 95;
        pst[65][186] = 57;
        pst[65][187] = 65;
        pst[65][195] = 65;
        pst[65][196] = 92;
        pst[65][197] = 57;
        pst[65][198] = 104;
        pst[65][199] = 55;
        pst[65][200] = 104;
        pst[65][201] = 57;
        pst[65][202] = 92;
        pst[65][203] = 65;
        pst[66][51] = 70;
        pst[66][52] = 39;
        pst[66][53] = 12;
        pst[66][54] = 35;
        pst[66][55] = 72;
        pst[66][56] = 35;
        pst[66][57] = 12;
        pst[66][58] = 39;
        pst[66][59] = 70;
        pst[66][67] = 65;
        pst[66][68] = 55;
        pst[66][69] = 15;
        pst[66][70] = 20;
        pst[66][71] = 68;
        pst[66][72] = 20;
        pst[66][73] = 15;
        pst[66][74] = 55;
        pst[66][75] = 65;
        pst[66][83] = 12;
        pst[66][84] = 41;
        pst[66][85] = 75;
        pst[66][86] = 41;
        pst[66][87] = 6;
        pst[66][88] = 41;
        pst[66][89] = 75;
        pst[66][90] = 41;
        pst[66][91] = 12;
        pst[66][99] = 27;
        pst[66][100] = 31;
        pst[66][101] = 70;
        pst[66][102] = 30;
        pst[66][103] = 25;
        pst[66][104] = 30;
        pst[66][105] = 70;
        pst[66][106] = 31;
        pst[66][107] = 27;
        pst[66][115] = 65;
        pst[66][116] = 35;
        pst[66][117] = 12;
        pst[66][118] = 35;
        pst[66][119] = 70;
        pst[66][120] = 35;
        pst[66][121] = 12;
        pst[66][122] = 35;
        pst[66][123] = 65;
        pst[66][131] = 60;
        pst[66][132] = 30;
        pst[66][133] = 31;
        pst[66][134] = 30;
        pst[66][135] = 65;
        pst[66][136] = 30;
        pst[66][137] = 31;
        pst[66][138] = 30;
        pst[66][139] = 60;
        pst[66][147] = 10;
        pst[66][148] = 35;
        pst[66][149] = 65;
        pst[66][150] = 35;
        pst[66][151] = 10;
        pst[66][152] = 35;
        pst[66][153] = 65;
        pst[66][154] = 35;
        pst[66][155] = 10;
        pst[66][163] = 35;
        pst[66][164] = 30;
        pst[66][165] = 49;
        pst[66][166] = 30;
        pst[66][167] = 43;
        pst[66][168] = 30;
        pst[66][169] = 49;
        pst[66][170] = 30;
        pst[66][171] = 35;
        pst[66][179] = 55;
        pst[66][180] = 27;
        pst[66][181] = 6;
        pst[66][182] = 27;
        pst[66][183] = 30;
        pst[66][184] = 27;
        pst[66][185] = 6;
        pst[66][186] = 27;
        pst[66][187] = 55;
        pst[66][195] = 35;
        pst[66][196] = 16;
        pst[66][197] = 40;
        pst[66][198] = 16;
        pst[66][199] = 20;
        pst[66][200] = 16;
        pst[66][201] = 40;
        pst[66][202] = 16;
        pst[66][203] = 35;
        pst[67][51] = 160;
        pst[67][52] = 160;
        pst[67][53] = 165;
        pst[67][54] = 157;
        pst[67][55] = 150;
        pst[67][56] = 157;
        pst[67][57] = 165;
        pst[67][58] = 160;
        pst[67][59] = 160;
        pst[67][67] = 133;
        pst[67][68] = 133;
        pst[67][69] = 131;
        pst[67][70] = 127;
        pst[67][71] = 134;
        pst[67][72] = 127;
        pst[67][73] = 131;
        pst[67][74] = 133;
        pst[67][75] = 133;
        pst[67][83] = 132;
        pst[67][84] = 132;
        pst[67][85] = 131;
        pst[67][86] = 126;
        pst[67][87] = 127;
        pst[67][88] = 126;
        pst[67][89] = 131;
        pst[67][90] = 132;
        pst[67][91] = 132;
        pst[67][99] = 131;
        pst[67][100] = 134;
        pst[67][101] = 134;
        pst[67][102] = 133;
        pst[67][103] = 135;
        pst[67][104] = 133;
        pst[67][105] = 134;
        pst[67][106] = 134;
        pst[67][107] = 131;
        pst[67][115] = 131;
        pst[67][116] = 131;
        pst[67][117] = 131;
        pst[67][118] = 131;
        pst[67][119] = 135;
        pst[67][120] = 131;
        pst[67][121] = 131;
        pst[67][122] = 131;
        pst[67][123] = 131;
        pst[67][131] = 130;
        pst[67][132] = 131;
        pst[67][133] = 134;
        pst[67][134] = 131;
        pst[67][135] = 135;
        pst[67][136] = 131;
        pst[67][137] = 134;
        pst[67][138] = 131;
        pst[67][139] = 130;
        pst[67][147] = 126;
        pst[67][148] = 126;
        pst[67][149] = 126;
        pst[67][150] = 126;
        pst[67][151] = 140;
        pst[67][152] = 126;
        pst[67][153] = 126;
        pst[67][154] = 126;
        pst[67][155] = 126;
        pst[67][163] = 127;
        pst[67][164] = 126;
        pst[67][165] = 127;
        pst[67][166] = 129;
        pst[67][167] = 140;
        pst[67][168] = 129;
        pst[67][169] = 126;
        pst[67][170] = 126;
        pst[67][171] = 127;
        pst[67][179] = 126;
        pst[67][180] = 127;
        pst[67][181] = 128;
        pst[67][182] = 128;
        pst[67][183] = 135;
        pst[67][184] = 128;
        pst[67][185] = 128;
        pst[67][186] = 127;
        pst[67][187] = 126;
        pst[67][195] = 126;
        pst[67][196] = 127;
        pst[67][197] = 127;
        pst[67][198] = 129;
        pst[67][199] = 129;
        pst[67][200] = 129;
        pst[67][201] = 127;
        pst[67][202] = 127;
        pst[67][203] = 126;
        pst[75][51] = 0;
        pst[75][52] = 0;
        pst[75][53] = 0;
        pst[75][54] = 0;
        pst[75][55] = 0;
        pst[75][56] = 0;
        pst[75][57] = 0;
        pst[75][58] = 0;
        pst[75][59] = 0;
        pst[75][67] = 0;
        pst[75][68] = 0;
        pst[75][69] = 0;
        pst[75][70] = 0;
        pst[75][71] = 0;
        pst[75][72] = 0;
        pst[75][73] = 0;
        pst[75][74] = 0;
        pst[75][75] = 0;
        pst[75][83] = 0;
        pst[75][84] = 0;
        pst[75][85] = 0;
        pst[75][86] = 0;
        pst[75][87] = 0;
        pst[75][88] = 0;
        pst[75][89] = 0;
        pst[75][90] = 0;
        pst[75][91] = 0;
        pst[75][99] = 0;
        pst[75][100] = 0;
        pst[75][101] = 0;
        pst[75][102] = 0;
        pst[75][103] = 0;
        pst[75][104] = 0;
        pst[75][105] = 0;
        pst[75][106] = 0;
        pst[75][107] = 0;
        pst[75][115] = 0;
        pst[75][116] = 0;
        pst[75][117] = 0;
        pst[75][118] = 0;
        pst[75][119] = 0;
        pst[75][120] = 0;
        pst[75][121] = 0;
        pst[75][122] = 0;
        pst[75][123] = 0;
        pst[75][131] = 0;
        pst[75][132] = 0;
        pst[75][133] = 0;
        pst[75][134] = 0;
        pst[75][135] = 0;
        pst[75][136] = 0;
        pst[75][137] = 0;
        pst[75][138] = 0;
        pst[75][139] = 0;
        pst[75][147] = 0;
        pst[75][148] = 0;
        pst[75][149] = 0;
        pst[75][150] = 0;
        pst[75][151] = 0;
        pst[75][152] = 0;
        pst[75][153] = 0;
        pst[75][154] = 0;
        pst[75][155] = 0;
        pst[75][163] = 0;
        pst[75][164] = 0;
        pst[75][165] = 0;
        pst[75][166] = 2340;
        pst[75][167] = 2420;
        pst[75][168] = 2340;
        pst[75][169] = 0;
        pst[75][170] = 0;
        pst[75][171] = 0;
        pst[75][179] = 0;
        pst[75][180] = 0;
        pst[75][181] = 0;
        pst[75][182] = 2420;
        pst[75][183] = 2460;
        pst[75][184] = 2420;
        pst[75][185] = 0;
        pst[75][186] = 0;
        pst[75][187] = 0;
        pst[75][195] = 0;
        pst[75][196] = 0;
        pst[75][197] = 0;
        pst[75][198] = 2460;
        pst[75][199] = 2500;
        pst[75][200] = 2460;
        pst[75][201] = 0;
        pst[75][202] = 0;
        pst[75][203] = 0;
        pst[78][51] = 148;
        pst[78][52] = 140;
        pst[78][53] = 140;
        pst[78][54] = 146;
        pst[78][55] = 140;
        pst[78][56] = 146;
        pst[78][57] = 140;
        pst[78][58] = 140;
        pst[78][59] = 158;
        pst[78][67] = 140;
        pst[78][68] = 146;
        pst[78][69] = 165;
        pst[78][70] = 137;
        pst[78][71] = 140;
        pst[78][72] = 137;
        pst[78][73] = 165;
        pst[78][74] = 146;
        pst[78][75] = 140;
        pst[78][83] = 137;
        pst[78][84] = 133;
        pst[78][85] = 134;
        pst[78][86] = 145;
        pst[78][87] = 149;
        pst[78][88] = 145;
        pst[78][89] = 134;
        pst[78][90] = 133;
        pst[78][91] = 137;
        pst[78][99] = 133;
        pst[78][100] = 158;
        pst[78][101] = 142;
        pst[78][102] = 155;
        pst[78][103] = 140;
        pst[78][104] = 155;
        pst[78][105] = 142;
        pst[78][106] = 158;
        pst[78][107] = 133;
        pst[78][115] = 130;
        pst[78][116] = 140;
        pst[78][117] = 145;
        pst[78][118] = 153;
        pst[78][119] = 154;
        pst[78][120] = 153;
        pst[78][121] = 145;
        pst[78][122] = 140;
        pst[78][123] = 130;
        pst[78][131] = 130;
        pst[78][132] = 138;
        pst[78][133] = 141;
        pst[78][134] = 142;
        pst[78][135] = 142;
        pst[78][136] = 142;
        pst[78][137] = 141;
        pst[78][138] = 138;
        pst[78][139] = 130;
        pst[78][147] = 122;
        pst[78][148] = 124;
        pst[78][149] = 128;
        pst[78][150] = 125;
        pst[78][151] = 128;
        pst[78][152] = 125;
        pst[78][153] = 128;
        pst[78][154] = 124;
        pst[78][155] = 122;
        pst[78][163] = 123;
        pst[78][164] = 123;
        pst[78][165] = 124;
        pst[78][166] = 125;
        pst[78][167] = 135;
        pst[78][168] = 125;
        pst[78][169] = 124;
        pst[78][170] = 123;
        pst[78][171] = 123;
        pst[78][179] = 99;
        pst[78][180] = 105;
        pst[78][181] = 99;
        pst[78][182] = 99;
        pst[78][183] = 99;
        pst[78][184] = 99;
        pst[78][185] = 99;
        pst[78][186] = 105;
        pst[78][187] = 99;
        pst[78][195] = 85;
        pst[78][196] = 85;
        pst[78][197] = 90;
        pst[78][198] = 105;
        pst[78][199] = 95;
        pst[78][200] = 105;
        pst[78][201] = 90;
        pst[78][202] = 85;
        pst[78][203] = 85;
        pst[80][51] = 75;
        pst[80][52] = 84;
        pst[80][53] = 85;
        pst[80][54] = 87;
        pst[80][55] = 87;
        pst[80][56] = 87;
        pst[80][57] = 85;
        pst[80][58] = 84;
        pst[80][59] = 75;
        pst[80][67] = 75;
        pst[80][68] = 84;
        pst[80][69] = 89;
        pst[80][70] = 101;
        pst[80][71] = 123;
        pst[80][72] = 101;
        pst[80][73] = 89;
        pst[80][74] = 84;
        pst[80][75] = 75;
        pst[80][83] = 60;
        pst[80][84] = 74;
        pst[80][85] = 74;
        pst[80][86] = 79;
        pst[80][87] = 97;
        pst[80][88] = 79;
        pst[80][89] = 74;
        pst[80][90] = 74;
        pst[80][91] = 60;
        pst[80][99] = 47;
        pst[80][100] = 47;
        pst[80][101] = 47;
        pst[80][102] = 49;
        pst[80][103] = 60;
        pst[80][104] = 49;
        pst[80][105] = 47;
        pst[80][106] = 47;
        pst[80][107] = 47;
        pst[80][115] = 34;
        pst[80][116] = 35;
        pst[80][117] = 37;
        pst[80][118] = 37;
        pst[80][119] = 39;
        pst[80][120] = 37;
        pst[80][121] = 37;
        pst[80][122] = 35;
        pst[80][123] = 34;
        pst[80][131] = 24;
        pst[80][132] = 35;
        pst[80][133] = 20;
        pst[80][134] = 27;
        pst[80][135] = 29;
        pst[80][136] = 27;
        pst[80][137] = 20;
        pst[80][138] = 35;
        pst[80][139] = 24;
        pst[80][147] = 13;
        pst[80][148] = 13;
        pst[80][149] = 14;
        pst[80][150] = 14;
        pst[80][151] = 15;
        pst[80][152] = 14;
        pst[80][153] = 14;
        pst[80][154] = 13;
        pst[80][155] = 13;
        pst[80][163] = 5;
        pst[80][164] = 5;
        pst[80][165] = 6;
        pst[80][166] = 4;
        pst[80][167] = 5;
        pst[80][168] = 4;
        pst[80][169] = 6;
        pst[80][170] = 5;
        pst[80][171] = 5;
        pst[80][179] = 4;
        pst[80][180] = 4;
        pst[80][181] = 5;
        pst[80][182] = -6;
        pst[80][183] = -10;
        pst[80][184] = -6;
        pst[80][185] = 5;
        pst[80][186] = 4;
        pst[80][187] = 4;
        pst[80][195] = 2;
        pst[80][196] = 2;
        pst[80][197] = 3;
        pst[80][198] = -9;
        pst[80][199] = -12;
        pst[80][200] = -9;
        pst[80][201] = 3;
        pst[80][202] = 2;
        pst[80][203] = 2;
        pst[82][51] = 376;
        pst[82][52] = 378;
        pst[82][53] = 378;
        pst[82][54] = 369;
        pst[82][55] = 383;
        pst[82][56] = 369;
        pst[82][57] = 378;
        pst[82][58] = 378;
        pst[82][59] = 376;
        pst[82][67] = 360;
        pst[82][68] = 362;
        pst[82][69] = 362;
        pst[82][70] = 369;
        pst[82][71] = 393;
        pst[82][72] = 369;
        pst[82][73] = 362;
        pst[82][74] = 362;
        pst[82][75] = 360;
        pst[82][83] = 360;
        pst[82][84] = 362;
        pst[82][85] = 362;
        pst[82][86] = 369;
        pst[82][87] = 366;
        pst[82][88] = 369;
        pst[82][89] = 362;
        pst[82][90] = 362;
        pst[82][91] = 360;
        pst[82][99] = 360;
        pst[82][100] = 362;
        pst[82][101] = 362;
        pst[82][102] = 369;
        pst[82][103] = 366;
        pst[82][104] = 369;
        pst[82][105] = 362;
        pst[82][106] = 362;
        pst[82][107] = 360;
        pst[82][115] = 358;
        pst[82][116] = 361;
        pst[82][117] = 361;
        pst[82][118] = 369;
        pst[82][119] = 366;
        pst[82][120] = 369;
        pst[82][121] = 361;
        pst[82][122] = 361;
        pst[82][123] = 358;
        pst[82][131] = 358;
        pst[82][132] = 352;
        pst[82][133] = 362;
        pst[82][134] = 369;
        pst[82][135] = 365;
        pst[82][136] = 369;
        pst[82][137] = 362;
        pst[82][138] = 354;
        pst[82][139] = 358;
        pst[82][147] = 354;
        pst[82][148] = 354;
        pst[82][149] = 354;
        pst[82][150] = 369;
        pst[82][151] = 364;
        pst[82][152] = 369;
        pst[82][153] = 354;
        pst[82][154] = 354;
        pst[82][155] = 354;
        pst[82][163] = 348;
        pst[82][164] = 354;
        pst[82][165] = 354;
        pst[82][166] = 369;
        pst[82][167] = 352;
        pst[82][168] = 369;
        pst[82][169] = 354;
        pst[82][170] = 354;
        pst[82][171] = 348;
        pst[82][179] = 350;
        pst[82][180] = 358;
        pst[82][181] = 356;
        pst[82][182] = 369;
        pst[82][183] = 340;
        pst[82][184] = 369;
        pst[82][185] = 356;
        pst[82][186] = 358;
        pst[82][187] = 350;
        pst[82][195] = 354;
        pst[82][196] = 356;
        pst[82][197] = 354;
        pst[82][198] = 362;
        pst[82][199] = 330;
        pst[82][200] = 362;
        pst[82][201] = 354;
        pst[82][202] = 356;
        pst[82][203] = 354;
        for(char c : MINGZI){
            if(c == 'K'){
                continue;
            }
            int sum = 0;
            for(int i = 51; i <= 203; ++i){
                if((i & 15) < 3 || (i & 15) > 11) { continue; }
                sum += pst[(int)c][i];
            }
            pst[(int)c][0] = sum / 90;
        }
    }

    void destroy_pst(){
        for(int i = 0; i < 91; ++i){
            delete[] pst[i];
        }
        delete[] pst;
    }

    void initialize_L1(){
        L1 = new unsigned char*[256];
        for(int i = 0; i < 256; ++i){
            L1[i] = new unsigned char[256];
            memset(L1[i], 0, 256 * sizeof(unsigned char));
            for(int j = 0; j < 256; ++j){
                int x1 = i >> 4;
                int x2 = j >> 4;
                int y1 = i & 15;
                int y2 = j & 15;
                L1[i][j] = (unsigned char)(abs(x2 - x1) + abs(y2 - y1));
            }
        }
    }

    void destroy_L1(){
        for(int i = 0; i < 256; ++i){
            delete[] L1[i];
        }
        delete[] L1;
    }

    void initialize_dir(){
        _dir = new char*[91];
        for(int i = 0; i < 91; ++i){
            _dir[i] = new char[8];
            memset(_dir[i], 0, 8 * sizeof(char));
        }
        _dir[(int)'P'][0] = NORTH;
        _dir[(int)'P'][1] = WEST;
        _dir[(int)'P'][2] = EAST;
        
        _dir[(int)'I'][0] = NORTH;
        
        _dir[(int)'N'][0] = NORTH + NORTH + EAST; //N+N+E, E+N+E, E+S+E, S+S+E, S+S+W, W+S+W, W+N+W, N+N+W
        _dir[(int)'N'][1] = EAST + NORTH + EAST;
        _dir[(int)'N'][2] = EAST + SOUTH + EAST;
        _dir[(int)'N'][3] = SOUTH + SOUTH + EAST;
        _dir[(int)'N'][4] = SOUTH + SOUTH + WEST;
        _dir[(int)'N'][5] = WEST + SOUTH + WEST;
        _dir[(int)'N'][6] = WEST + NORTH + WEST;
        _dir[(int)'N'][7] = NORTH + NORTH + WEST;

        _dir[(int)'E'][0] = NORTH + NORTH + EAST; //N+N+E, E+N+E, E+S+E, S+S+E, S+S+W, W+S+W, W+N+W, N+N+W
        _dir[(int)'E'][1] = EAST + NORTH + EAST;
        _dir[(int)'E'][2] = WEST + NORTH + WEST;
        _dir[(int)'E'][3] = NORTH + NORTH + WEST;
        
        _dir[(int)'B'][0] = 2 * NORTH + 2 * EAST;//2 * N + 2 * E, 2 * S + 2 * E, 2 * N + 2 * W, 2 * S + 2 * W
        _dir[(int)'B'][1] = 2 * SOUTH + 2 * EAST;
        _dir[(int)'B'][2] = 2 * NORTH + 2 * WEST;
        _dir[(int)'B'][3] = 2 * SOUTH + 2 * WEST;
        
        _dir[(int)'F'][0] = 2 * NORTH + 2 * EAST;
        _dir[(int)'F'][1] = 2 * NORTH + 2 * WEST;
        
        _dir[(int)'R'][0] = NORTH;
        _dir[(int)'R'][1] = EAST;
        _dir[(int)'R'][2] = SOUTH;
        _dir[(int)'R'][3] = WEST;

        _dir[(int)'D'][0] = NORTH;
        _dir[(int)'D'][1] = EAST;
        _dir[(int)'D'][2] = WEST;

        _dir[(int)'C'][0] = NORTH;
        _dir[(int)'C'][1] = EAST;
        _dir[(int)'C'][2] = SOUTH;
        _dir[(int)'C'][3] = WEST;

        _dir[(int)'H'][0] = NORTH;
        _dir[(int)'H'][1] = EAST;
        _dir[(int)'H'][2] = SOUTH;
        _dir[(int)'H'][3] = WEST;

        _dir[(int)'A'][0] = NORTH + EAST;
        _dir[(int)'A'][1] = SOUTH + EAST;
        _dir[(int)'A'][2] = NORTH + WEST;
        _dir[(int)'A'][3] = SOUTH + WEST;

        _dir[(int)'G'][0] = NORTH + EAST;
        _dir[(int)'G'][1] = NORTH + WEST;

        _dir[(int)'K'][0] = NORTH;
        _dir[(int)'K'][1] = EAST;
        _dir[(int)'K'][2] = SOUTH;
        _dir[(int)'K'][3] = WEST;
    }

    void destroy_dir(){
        for(int i = 0; i < 91; ++i){
            delete[] _dir[i];
        }
        delete[] _dir;
    }

    std::string __Adapter(std::string& board);
    bool __InnerHint(int depth, unsigned char& src, unsigned char& dst, py::dict* valdict);
};

#endif