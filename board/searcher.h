//Reference: https://github.com/junxiaosong/AlphaZero_Gomoku/blob/master/mcts_alphaZero.py

#ifndef searcher_h
#define searcher_h
#include <math.h>
#include <vector>
#include "god.h"
#include "../global/global.h"

#define DIV(X, Y) ((Y) ? ((X) / (Y)) : 0.0f)
struct Node{
    bool deleted, turn;
    char state[MAX];
    float value, prob;
    size_t n_visits;
    Node* parent;
    std::unordered_map<std::string, Node*> children;
    Node()=delete;
    Node(bool turn, float prob, Node* parent):deleted(false), turn(turn), value(0.0f), prob(prob), n_visits(0), parent(parent){}

    void CopyState(char state[MAX]){
        strncpy(this->state, state, MAX-1);
        this->state[MAX-1] = '\0';
    }

    void Free(Node* n){
        deleted = true;
        for(auto child = n -> children.begin(); child != n -> children.end(); ++child){
            if(child -> second){
                Free(child -> second);
            }
        }
        children.clear();
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
            return DIV(value, n_visits);
        }
        return DIV(value, n_visits) + c_puct * prob * sqrt(parent -> n_visits)/(1 + n_visits);
    }

    void expand(std::unordered_map<std::string, float>& probs){
        for(auto prob = probs.begin(); prob != probs.end(); ++prob){
            children[prob -> first] = new Node(!turn, prob -> second, this);
        }
    }

    std::string info(){
        return "turn: " + std::to_string(turn) + ", state: " + std::string(state) + ", prob: " + std::to_string(prob);
    }
};

struct Searcher{
    God g;
    Node *root, *cur;
    float c_puct;
    std::unordered_map<std::string, float> childrenvalue;
    Searcher():root(NULL), cur(NULL), c_puct(0.0f){}

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
        CopyState(root);
        cur = root;
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

    void expand(Node* n, std::unordered_map<std::string, float>& probs){
        if(!n){
            return;
        }
        CopyState(n);
        n -> expand(probs);
    }

    void expand(py::dict probs){
        std::unordered_map<std::string, float> _probs;
        for(auto item: probs){
            _probs[item.first.cast<std::string>()] = item.second.cast<float>();
        }
        expand(cur, _probs);
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

    void Move(std::string s){
        if(cur && cur -> children.find(s) != cur -> children.end()){
            cur = cur -> children[s];
        }
        g.board_pointer -> Move(s);
    }

};
#endif