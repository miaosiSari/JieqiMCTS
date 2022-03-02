//Reference: https://github.com/junxiaosong/AlphaZero_Gomoku/blob/master/mcts_alphaZero.py

#ifndef searcher_h
#define searcher_h
#include <math.h>
#include <vector>
#include <pybind11/numpy.h>
#include "god.h"
#include "../global/global.h"
#define NEW new board::AIBoard4(state, turn, 0, di, tptable, pst, L1, _dir)
#define DIV(X, Y) ((Y) ? ((X) / (Y)) : 0.0f)
#define F(X, Y, Z) (((X) * 90) + ((Y) * 9) + (Z))
struct Node{
    bool deleted, turn;
    char state[MAX];
    unsigned char di[2][123];
    float value, prob;
    size_t n_visits;
    Node* parent;
    tp* tptable;
    short** pst;
    unsigned char** L1;
    char** _dir;
    int flag;
    std::unordered_map<std::string, Node*> children;
    Node()=delete;
    Node(bool turn, float prob, Node* parent):deleted(false), turn(turn), value(0.0f), prob(prob), n_visits(0), parent(parent), \
        tptable(NULL), pst(NULL), L1(NULL), _dir(NULL), flag(0){}

    void CopyState(char state[MAX]){
        strncpy(this->state, state, MAX-1);
        this->state[MAX-1] = '\0';
    }

    void CopyDi(unsigned char di[2][123]){
        this -> di[1][(int)'R'] = di[1][(int)'R'];
        this -> di[1][(int)'N'] = di[1][(int)'N'];
        this -> di[1][(int)'B'] = di[1][(int)'B'];
        this -> di[1][(int)'A'] = di[1][(int)'A'];
        this -> di[1][(int)'C'] = di[1][(int)'C'];
        this -> di[1][(int)'P'] = di[1][(int)'P'];
        this -> di[1][(int)'K'] = 0;
        this -> di[0][(int)'r'] = di[0][(int)'r'];
        this -> di[0][(int)'n'] = di[0][(int)'n'];
        this -> di[0][(int)'b'] = di[0][(int)'b'];
        this -> di[0][(int)'a'] = di[0][(int)'a'];
        this -> di[0][(int)'c'] = di[0][(int)'c'];
        this -> di[0][(int)'p'] = di[0][(int)'p'];
        this -> di[0][(int)'k'] = 0;
    }

    void Free(Node* n){
        deleted = true;
        for(auto child = n -> children.begin(); child != n -> children.end(); ++child){
            if(child -> second){
                Free(child -> second);
                free(child -> second);
            }
        }
    }

    void Reset(){
        deleted = false;
        value = 0.0f;
        n_visits = 0;
    }

    ~Node(){
        Free(this);
    }

    size_t size(){
        return children.size();
    }

    bool empty(){
        return children.empty();
    }

    bool haspa(){
        return (parent != NULL);
    }

    py::object get(std::string s){
        if(children.find(s) != children.end()){
            return py::cast(*children[s]);
        }
        return py::none();
    }

    py::object pa(){
        if(parent){
            return py::cast(*parent);
        }
        return py::none();
    }

    float _div(float value, size_t n_visits){
        return (n_visits > 0 ? value/n_visits : 0.0f);
    }

    float UCT(float c_puct){
        if(!parent){
            return -DIV(value, n_visits);
        }
        return -DIV(value, n_visits) + c_puct * prob * sqrt(parent -> n_visits)/(1 + n_visits);
    }

    void expand(std::unordered_map<std::string, float>& probs){
        for(auto prob = probs.begin(); prob != probs.end(); ++prob){
            children[prob -> first] = new Node(!turn, prob -> second, this);
        }
    }

    void expand(py::dict probs){
        std::unordered_map<std::string, float> _probs;
        for(auto item: probs){
            _probs[item.first.cast<std::string>()] = item.second.cast<float>();
        }
        expand(_probs);
    }

    py::list getdi(){
        py::list dilist;
        DILIST(dilist, di);
        return dilist;
    }

    py::tuple meta(){
        constexpr size_t size = 26*10*9;
        float* foo = new float[size];
        memset(foo, 0.0f, size*sizeof(float));
        py::capsule free_when_done(foo, [](void* f){
            float* foo = reinterpret_cast<float *>(f);
            delete[] foo;
        });
        py::list di = (flag & 1)? getdi(): py::list();
        if(flag & 2){
            for(size_t i = 51; i <= 203; ++i){
                int x = (i >> 4);
                int y = (i & 15);
                if(y < 3 || y > 11){
                    continue;
                }
                x = 12 - x;
                y -= 3;
                switch(state[i]){
                    case 'R': foo[F(0, x, y)] = 1.0f; break;
                    case 'N': foo[F(1, x, y)] = 1.0f; break;
                    case 'B': foo[F(2, x, y)] = 1.0f; break;
                    case 'A': foo[F(3, x, y)] = 1.0f; break;
                    case 'K': foo[F(4, x, y)] = 1.0f; break;
                    case 'C': foo[F(5, x, y)] = 1.0f; break;
                    case 'P': foo[F(6, x, y)] = 1.0f; break;
                    case 'D': foo[F(7, x, y)] = 1.0f; break;
                    case 'E': foo[F(8, x, y)] = 1.0f; break;
                    case 'F': foo[F(9, x, y)] = 1.0f; break;
                    case 'G': foo[F(10, x, y)] = 1.0f; break;
                    case 'H': foo[F(11, x, y)] = 1.0f; break;
                    case 'I': foo[F(12, x, y)] = 1.0f; break;

                    case 'r': foo[F(13, x, y)] = 1.0f; break;
                    case 'n': foo[F(14, x, y)] = 1.0f; break;
                    case 'b': foo[F(15, x, y)] = 1.0f; break;
                    case 'a': foo[F(16, x, y)] = 1.0f; break;
                    case 'k': foo[F(17, x, y)] = 1.0f; break;
                    case 'c': foo[F(18, x, y)] = 1.0f; break;
                    case 'p': foo[F(19, x, y)] = 1.0f; break;
                    case 'd': foo[F(20, x, y)] = 1.0f; break;
                    case 'e': foo[F(21, x, y)] = 1.0f; break;
                    case 'f': foo[F(22, x, y)] = 1.0f; break;
                    case 'g': foo[F(23, x, y)] = 1.0f; break;
                    case 'h': foo[F(24, x, y)] = 1.0f; break;
                    case 'i': foo[F(25, x, y)] = 1.0f; break;
                    
                    default: break;
                }
            }
        }
        return py::make_tuple(py::array_t<float>(
            {26, 10, 9}, //shape
            {10*9*4, 9*4, 4}, //stride
            foo, //the data pointer
            free_when_done
        ), di, py::int_(flag));
    }

    std::string getstate(){
        return state;
    }

    py::tuple AIHint(int depth){
        std::function<std::string(unsigned char)> translate_single = [](unsigned char i) -> std::string{
            int x1 = 12 - (i >> 4);
            int y1 = (i & 15) - 3;
            std::string ret = "  ";
            ret[0] = 'a' + y1;
            ret[1] = '0' + x1;
            return ret;
        };
        std::function<std::string(unsigned char, unsigned char)> translate_ucci = [&](unsigned char src, unsigned char dst) -> std::string{
            return translate_single(src) + translate_single(dst);
        };

        board::Thinker* ai = NEW;
        unsigned char src = 0, dst = 0;
        py::dict valdict;
        bool thinkres = ai -> Think(depth, src, dst, &valdict);
        return py::make_tuple(py::int_(src), py::int_(dst), py::str(translate_ucci(src, dst)), py::bool_(thinkres), valdict);
    }

    std::string pos(bool print) const{
        std::string ret;
        if(turn){
            ret += "RED\n";   
        }else{
            ret += "BLACK\n";
        }
        for(int x = 3; x <= 12; ++x){
            ret += std::to_string(12 - x);
            ret += ' ';
            for(int y = 3; y <= 11; ++y){
                const char c = state[16*x + y];
                ret += c;
            }
            ret += '\n';
        }
        ret += "  abcdefghi\n";
        if(print){
            std::cout << ret << "\n";
        }
        return ret;
    }

    std::string info(){
        return "turn: " + std::to_string(turn) + ", state: " + std::string(state) + ", prob: " + std::to_string(prob);
    }

    std::string info2(){
        return "turn: " + std::to_string(turn) + ", prob: " + std::to_string(prob);
    }
};

struct Searcher{
    God g;
    Node *root, *cur;
    float c_puct;
    std::unordered_map<std::string, float> childrenvalue;
    Searcher():root(NULL), cur(NULL), c_puct(0.0f), node_count(0){}

    bool Initialize(bool random, bool turn, std::string board, float c_puct, float prob){
        if(root){
            delete root;
            root = NULL;
        }
        this -> c_puct = c_puct;
        bool res = g.Initialize(random, turn, board);
        root = new (std::nothrow) Node(g.board_pointer -> turn, prob, NULL);
        if(!root){
            throw Exception{"root is NULL!"};
        }
        copy(root, 3);
        cur = root;
        curtmp = cur;
        return res;
    }

    ~Searcher(){
        if(root){
            delete root;
            root = NULL;
        }
    }

    py::object get(bool return_root){
        if(return_root){
            if(root){
                return py::cast(*root);
            }
        }else{
            if(cur){
                return py::cast(*cur);
            }
        }
        return py::none();
    }

    float UCT(Node* n, std::string& index){
        float maxval = -std::numeric_limits<float>::infinity();
        childrenvalue.clear();
        for(auto it = n -> children.begin(); it != n -> children.end(); ++it){
            if(!it -> second){
                continue;
            }
            float val = it -> second -> UCT(c_puct);
            childrenvalue[it -> first] = val;
            if(val > maxval){
                maxval = val;
                index = it -> first;
            }
        }
        return maxval;
    }

    py::dict UCT(){
        py::dict returndict;
        std::string index;
        float maxval = UCT(cur, index);
        for(auto it = childrenvalue.begin(); it != childrenvalue.end(); ++it){
            returndict[py::str(it -> first)] = it->second;
        }
        returndict["max"] = maxval;
        returndict["index"] = index;
        return returndict;
    }

    void CopyState(Node* n){
        n -> CopyState(g.board_pointer -> turn?g.board_pointer -> state_red : g.board_pointer ->state_black);
    }

    void CopyDi(Node* n){
        n -> CopyDi(g.board_pointer -> turn?g.board_pointer -> di_red:g.board_pointer -> di_black);
    }

    void copy(Node* n, const int flag){
        n -> flag = flag;
        if(flag & 1){
            CopyState(n);
        }
        if(flag & 2){
            CopyDi(n);
        }
        n -> tptable = g.tptable;
        n -> pst = g.pst;
        n -> L1 = g.L1;
        n -> _dir = g._dir;
    }
    
    void copy(const int flag, const bool copy_root){
        if(copy_root){
            if(root){
                copy(root, flag);
            }
        }
        else{
            if(cur){
                copy(cur, flag);
            }
        }
    }

    void expand(Node* n, std::unordered_map<std::string, float>& probs){
        if(!n){
            return;
        }
        copy(n, 3);
        n -> expand(probs);
    }

    void expand(py::dict probs){
        std::unordered_map<std::string, float> _probs;
        for(auto item: probs){
            _probs[item.first.cast<std::string>()] = item.second.cast<float>();
        }
        expand(cur, _probs);
    }

    py::object child(py::list uccis){
        Node* tmp = cur;
        for(py::handle obj: uccis){
            std::string ucci = obj.cast<std::string>();
            if(tmp && tmp -> children.find(ucci) != tmp -> children.end()){
                tmp = tmp -> children[ucci];
            }
            else{
                return py::none();
            }
        }
        if(tmp){
            return py::cast(*tmp);
        }
        return py::none();
    }

    void walk(py::list uccis){
        curtmp = cur;
        for(py::handle obj: uccis){
            std::string ucci = obj.cast<std::string>();
            if(cur && cur -> children.find(ucci) != cur -> children.end()){
                cur = cur -> children[ucci];
            }
            else{
                return;
            }
        }
    }

    void reset(){
        cur = curtmp;
    }

    std::string pos(bool print){
        return cur -> pos(print);
    }

    py::tuple AIHint(int depth){
        return cur -> AIHint(depth);
    }

    void __draw(Node* n, size_t level, std::string& prefix){
        std::string indent(2*level, '*');
        std::cout << indent << prefix << "\n";
        for(auto child = n -> children.begin(); child != n -> children.end(); ++child){
            if(!child -> second){
                continue;
            }
            std::string newprefix = "[id:" + std::to_string(node_count)  + "]" + child->first + "<" + child->second->info2() + ">";
            ++node_count;
            __draw(child->second, level+1, newprefix);
        }
    }

    void draw(){
        std::string prefix = "[id:0 (root)]<" + root->info2() + ">";
        node_count = 1;
        __draw(root, 0, prefix);
    }

    void bp(Node* cur, float value){
        bool curturn = cur -> turn;
        Node* n = cur;
        while(n){
            n -> n_visits += 1;
            n -> value += (n -> turn == curturn ? value : -value);
            n = n -> parent;
        }
    }

    py::dict move(const int src, const int dst, bool check){
        return g.Move(src, dst, check);
    }

    py::dict move(const std::string s, bool check){
        return g.Move(s, check);
    }

    int flag(){
        if(cur){
            return cur -> flag;
        }
        return -1;
    }

    py::tuple meta(){
        return (cur ? cur -> meta() : py::make_tuple());
    }

    std::string getstate(){
        return cur ? cur -> state : std::string(256, ' ');
    }

    bool PrintPos(){
        return g.PrintPos();
    }

    short Rough(const float discount_factor){
        return g.Rough(discount_factor);
    }

private:
    size_t node_count;
    Node* curtmp;
};
#endif