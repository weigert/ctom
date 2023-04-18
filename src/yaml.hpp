#ifndef CTOM_YAML
#define CTOM_YAML

#include "ctom.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <string_view>

namespace ctom {
namespace yaml {

// Marshal

// Stream-Forwarding Operator

struct ostream {
    ostream(std::ostream& os):os(os){}
    std::ostream& os;
};


template<typename T>
ostream operator<<(ostream const& q, const T& t) {
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

    std::string to_string(){
        std::string str = "";
        for(auto& s: state){
            if(s == TAB) str = str+"  ";
            if(s == DASH) str = str+"- ";
        }
        return str;
    }
};

indent operator+(indent ind, indentstate state){
    ind.state.push_back(state);
    return ind;
}

ostream operator<<(ostream const& os, indent& i) {
    return os << i.to_string();
}

// Reference State

template<typename T>
struct set {
    indent ind;
    const char* key;
    T* t;
};

// Node-Type Entrypoints

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
        os<<s.key<<": ";
    }
    if(s.t != NULL) os<<s.t->value;
    return os << "\n";
}

template<arr_t T>
ostream operator<<(ostream const& os, set<T> s){

    if(s.key != NULL){
        os<<s.ind<<s.key<<":\n";
        for(auto& st: s.ind.state)
            st = TAB;
        s.ind = s.ind + TAB;
    }

    s.t->for_refs([&](auto&& ref){
        os << set{s.ind + DASH, NULL, ref.node.impl};
        for(auto& st: s.ind.state)
            st = TAB;
    });

    return os;
}

template<obj_t T>
ostream operator<<(ostream const& os, set<T> s){

    if(s.key != NULL){
        os<<s.ind<<s.key<<":\n";
        for(auto& st: s.ind.state)
            st = TAB;
        s.ind = s.ind + TAB;
    }

    s.t->for_refs([&](auto&& ref){
        os << set{s.ind, ref.key, ref.node.impl};
        for(auto& st: s.ind.state)
            st = TAB;
    });

    return os;

}

// Unmarshal

// Stream-Forwarding Operator

template<typename T>
struct pstream {
    pstream(T& t):t(t){}
    T& t;
};

// Operator Instance

struct pstream_t{} parse;

template<typename T>
pstream<T> operator<<(T& t, pstream_t&) {
    return pstream<T>(t);
}

// Node-Type Entrypoints

struct pset {
    indent ind;
    const char* key;
    std::string_view& t;
};

template<val_t T>
void operator<<(pstream<T> const& ps, std::string_view t){
    ps << pset{{}, NULL, t};
}

template<arr_t T>
void operator<<(pstream<T> const& ps, std::string_view t){
    ps << pset{{}, NULL, t};
}

template<obj_t T>
void operator<<(pstream<T> const& ps, std::string_view t){
    ps << pset{{}, NULL, t};
}

// Base Trim Operations

void trim_prefix(std::string_view& sv, std::string_view pre){
    if(!sv.starts_with(pre)) throw "failed to trim prefix";
    sv.remove_prefix(pre.size());
}

void trim_delim(std::string_view& sv, std::string_view delim){
    if(sv.starts_with(delim) ^ sv.ends_with(delim))
        throw "failed to trim delimiter";
    if(sv.starts_with(delim) && sv.ends_with(delim)){
        sv.remove_prefix(delim.size());
        sv.remove_suffix(delim.size());
    }
}

std::string_view find_delim(std::string_view& sv, std::string_view delim){
    return sv.substr(0, sv.find(delim)+1);
}

template<val_t T>
void operator<<(pstream<T> const& ps, pset s){

    // Find the Delimiter

    trim_prefix(s.t, s.ind.to_string());

    auto line = find_delim(s.t, "\n");
    std::cout<<line;

    trim_prefix(s.t, line);

}

template<arr_t T>
void operator<<(pstream<T> const& ps, pset s){

    if(s.key != NULL){

        trim_prefix(s.t, s.ind.to_string());

        auto line = find_delim(s.t, "\n");
        std::cout<<line;
        
        trim_prefix(s.t, line);

        for(auto& st: s.ind.state)
            st = TAB;
        s.ind = s.ind + TAB;

    }

    ps.t.for_refs([&](auto&& ref){
        *ref.node.impl << parse << pset{s.ind + DASH, NULL, s.t};
        for(auto& st: s.ind.state)
            st = TAB;
    });

}

template<obj_t T>
void operator<<(pstream<T> const& ps, pset s){

    if(s.key != NULL){

        trim_prefix(s.t, s.ind.to_string());

        auto line = find_delim(s.t, "\n");        
        std::cout<<line;

        trim_prefix(s.t, line);

        for(auto& st: s.ind.state)
            st = TAB;
        s.ind = s.ind + TAB;
    }

    ps.t.for_refs([&](auto&& ref){
        *ref.node.impl << parse << pset{s.ind, ref.key, s.t};
         for(auto& st: s.ind.state)
            st = TAB;
    });

}

}   // end of namespace yaml
}   // end of namespace ctom

#endif