#ifndef CTOM_JSON
#define CTOM_JSON

#include "ctom.hpp"
#include <string>
#include <iostream>
#include <vector>

namespace ctom {
namespace json {

// Stream-Forwarding Operator

struct ostream {
    ostream(std::ostream& os):os(os){}
    std::ostream& os;
};

template<typename T>
ostream operator<<(ostream const& q, T& t) {
    q.os << t;
    return q;
}

// Operator Instance

struct ostream_t{} emit;
ostream operator<<(std::ostream& os, ostream_t&) {
    return ostream(os);
}

// Indentation State

enum indentstate {
    TAB,
    DASH
};

struct indent {
    std::vector<indentstate> state;
    indent(){}
    indent(std::initializer_list<indentstate> s){
        state = s;
    }
};

indent operator+(indent ind, indentstate state){
    ind.state.push_back(state);
    return ind;
}

ostream operator<<(ostream const& os, indent& i) {
    for(auto& s: i.state){
        if(s == TAB) os << "  ";
        if(s == DASH) os << "- ";
    }
    return os;
}

// Reference State

template<typename T>
struct set {
    indent ind;
    const char* key;
    T* t;
    bool last = true;
};

template<val_t T>
ostream operator<<(ostream const& os, T& t){
    return os << set{{}, NULL, &t};
}

template<arr_t T>
ostream operator<<(ostream const& os, T& t){
    return os << set{{}, NULL, &t};
}

template<obj_t T>
ostream operator<<(ostream const& os, T& t){
    return os << set{{}, NULL, &t};
}

// Marshal Implementation

template<val_t T>
ostream operator<<(ostream const& os, set<T> s){
    os<<s.ind;
    if(s.key != NULL){
        os<<"\""<<s.key<<"\": ";
    }
    if(s.t != NULL) os<<"\""<<s.t->value<<"\"";
    else os<<"\"\"";
    if(!s.last) os<<",";
    return os << "\n";
}

template<arr_t T>
ostream operator<<(ostream const& os, set<T> s){

    os<<s.ind;
    if(s.key != NULL){
        os<<"\""<<s.key<<"\": ";
    }
    os<<"[\n";

    int n = 0;
    s.t->for_refs([&](auto&& ref){
        os << set{s.ind+TAB, NULL, ref.node.impl, (n == s.t->size-1)};
        n++;
    });

    os<<s.ind<<"]";
    if(!s.last) os<<",";
    return os<<"\n";
}

template<obj_t T>
ostream operator<<(ostream const& os, set<T> s){

    os<<s.ind;
    if(s.key != NULL){
        os<<"\""<<s.key<<"\": ";
    }
    os<<"{\n";

    int n = 0;
    s.t->for_refs([&](auto&& ref){
        os << set{s.ind+TAB, ref.key, ref.node.impl, (n == s.t->size-1)};
        n++;
    });

    os<<s.ind<<"}";
    if(!s.last) os<<",";
    return os<<"\n";

}

}   // end of namespace yaml
}   // end of namespace ctom

#endif