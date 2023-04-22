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


// Custom Exception

struct parse_exception: public std::exception {
    std::string msg;
    explicit parse_exception(std::string _msg):msg{_msg}{};
    const char* what() const noexcept override {
        return msg.c_str();
    }
};








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

std::string_view pre_delim(std::string_view& sv, std::string_view delim){
    return sv.substr(0, sv.find(delim));
}

std::string_view post_delim(std::string_view& sv, std::string_view delim){
    return sv.substr(sv.find(delim));
}

/*
1. Extract Line
2. Remove Comment
3. Extract Key / Val
4. Check
5. Parse
*/

template<val_t T>
void operator<<(pstream<T> const& ps, pset s){

    // Find the Delimiter

    auto line = pre_delim(s.t, "\n");
    trim_prefix(s.t, line);
    trim_prefix(s.t, "\n");

    // Extract Key

    trim_prefix(line, s.ind.to_string());
    line = pre_delim(line, "#");
  
    auto key = pre_delim(line, ":");
  //  auto val = post_delim(line, ":");
  //  trim_prefix(val, ":");

    if(key.find_first_not_of(" ") != 0){
        throw parse_exception("unexpected indent");
    }

    // Trim Whitespace

    key.remove_suffix(key.size()-key.find_last_not_of(" ")-1);
    trim_delim(key, "\"");

    if(s.key != NULL && s.key != key){
        throw parse_exception("unexpected key");
    }

    std::cout<<key<<"\n";

}

template<arr_t T>
void operator<<(pstream<T> const& ps, pset s){

    if(s.key != NULL){

        auto line = pre_delim(s.t, "\n");
        trim_prefix(s.t, line);
        trim_prefix(s.t, "\n");

        trim_prefix(line, s.ind.to_string());
        line = pre_delim(line, "#");

        auto key = pre_delim(line, ":");
        std::cout<<key<<"\n";
 
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

        auto line = pre_delim(s.t, "\n");   
        trim_prefix(s.t, line);
        trim_prefix(s.t, "\n");

        // Extract Key

        trim_prefix(line, s.ind.to_string());
        line = pre_delim(line, "#");

        // Check Post-Colon

        auto val = post_delim(line, ":");
        trim_prefix(val, ":");
        std::cout<<"VAL "<<val<<std::endl;

        auto key = pre_delim(line, ":");
        std::cout<<key<<"\n";

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