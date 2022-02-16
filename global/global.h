#ifndef global_h
#define global_h

#define VERSION_MAX 2
#define MAX 257
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <string>
#include <regex>
#include <functional>
#include <iostream>
#include <type_traits>

#define INTR (int)'R'
#define INTN (int)'N'
#define INTB (int)'B'
#define INTA (int)'A'
#define INTC (int)'C'
#define INTP (int)'P'
#define INTr (int)'r'
#define INTn (int)'n'
#define INTb (int)'b'
#define INTa (int)'a'
#define INTc (int)'c'
#define INTp (int)'p'
void IntializeL1();

struct God;

template<typename S>
S select_random(std::multiset<S> &s, bool erase=false) {
   size_t n = rand() % s.size();
   auto it = std::begin(s);
   std::advance(it,n);
   if(erase){
      s.erase(it);
   }
   return *it;
}

namespace board{
   struct Thinker;
}

//GetWithDef: If K does not exist, return the default value!
template <typename K, typename V>
V GetWithDef(const  std::map<K,V>& m, const K  key, const V& defval) {
   typename std::map<K,V>::const_iterator it = m.find( key );
   if ( it == m.end() ) {
      return defval;
   }
   else {
      return it->second;
   }
}

template <typename K, typename V>
V GetWithDefUnordered(const std::unordered_map<K,V>& m, const K& key, const V& defval) {
   typename std::unordered_map<K,V>::const_iterator it = m.find(key);
   if(it == m.end()) {
      return defval;
   }
   else {
      return it->second;
   }
}

template<typename T>
inline void hash_combine(std::size_t& seed, const T& val)
{
   seed ^= val + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

//  taken from https://stackoverflow.com/a/7222201/916549
//
template<typename S, typename T>
struct myhash
{
   inline size_t operator()(const std::pair<S, T>& val) const
   {
      size_t seed = 0;
      hash_combine(seed, val.first);
      hash_combine(seed, val.second);
      return seed;
   }
};

template <typename T, typename U, typename V>
bool GreaterTuple(const std::tuple<T, U, V> &i, const std::tuple<T, U, V> &j) {
   return std::get<0>(i) > std::get<0>(j);
}

template<typename T>
class Singleton{
private:
   static T* _t;
public:
   static T* get();
   static void deleteT();
   static bool reset();
};

template<typename T>
T* Singleton<T>::_t = new (std::nothrow) T;

template<typename T>
T* Singleton<T>::get(){
   return _t;
}

template<typename T>
void Singleton<T>::deleteT(){
   delete Singleton<T>::_t;
}

template<typename T>
bool Singleton<T>::reset(){
   delete _t;
   _t = new (std::nothrow) T;
   return (_t != NULL);
}

struct InfoDict{
   bool islegal;
   bool win;
   char eat;
   int eat_type; //0:'.', 1:明子, 2:暗子
   char eat_check;
   InfoDict()=delete;
   InfoDict(const bool islegal, const bool win, const char eat, const int eat_type, const char eat_check):islegal(islegal), win(win), eat(eat), eat_type(eat_type), eat_check(eat_check){}
   ~InfoDict()=default;
};

struct Exception{
   std::string errorstr;
   const char* what() const{
      return errorstr.c_str();
   }
};


struct tp{
   uint64_t key;
   bool turn;
   int alphadepth;
   int betadepth;
   short alphaval;
   short betaval;
   short score;
   unsigned char src;
   unsigned char dst;
   int recordplace;
};

#endif
