#ifndef CTOM_YAML
#define CTOM_YAML

#include "ctom.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <string_view>
#include <charconv>

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
    else os<<"null";
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
    pstream(T* t):t(t){}
    T* t;
};

// Operator Instance

struct pstream_t{} parse;

template<typename T>
pstream<T> operator<<(T* t, pstream_t&) {
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
    if(!sv.starts_with(pre)) throw parse_exception("failed to trim prefix");
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
    if(sv.find(delim) == std::string_view::npos)
        return sv;
    return sv.substr(0, sv.find(delim));
}

std::string_view post_delim(std::string_view& sv, std::string_view delim){
    if(sv.find(delim) == std::string_view::npos)
        return "";
    return sv.substr(sv.find(delim)+1);
}

// Value-Parser!!


template<typename T>
void parse_val(T& t, std::string_view v){
    auto val = std::from_chars(v.data(), v.data() + v.size(), t);
    if(val.ec == std::errc::invalid_argument){
        throw parse_exception("failed to parse value");
    }
}

template<>
void parse_val<char>(char& t, std::string_view v){
    /*
    auto val = std::from_chars(v.data(), v.data() + v.size(), t);
    if(val.ec == std::errc::invalid_argument){
        throw parse_exception("failed to parse value");
    }*/
    if(v.size() != 1){
        throw parse_exception("invalid size for char");
    }
    t = v[0];
}

/*
1. Extract Line
2. Remove Comment, Whitespace
3. Extract Key / Val
4. Check
5. Parse
*/

std::string_view get_line(std::string_view& sv){
    if(sv.find("\n") == std::string_view::npos){
        auto line = sv;
        trim_prefix(sv, line);
        return line;
    }
    auto line = sv.substr(0, sv.find("\n"));
    trim_prefix(sv, line);
    trim_prefix(sv, "\n");
    return line;
}

void trim_whitespace(std::string_view& line){
    if(line.find_first_not_of(" \t") != std::string_view::npos)
        line.remove_prefix(line.find_first_not_of(" \t"));
    if(line.find_last_not_of(" \t") != std::string_view::npos)
        line.remove_suffix(line.size()-line.find_last_not_of(" \t")-1);
}

void trim_line(std::string_view& line, indent& ind){
    trim_prefix(line, ind.to_string());
    line = pre_delim(line, "#");
}

std::string_view get_key(std::string_view line){
    if(line.find(":") != std::string_view::npos){
        auto key = pre_delim(line, ":");
        trim_whitespace(key);
        trim_delim(key, "\"");
        return key;
    }
    return "";
}

std::string_view get_val(std::string_view line){
    if(line.find(":") != std::string_view::npos){
        auto val = post_delim(line, ":");
        trim_whitespace(val);
        trim_delim(val, "\"");
        return val;
    }
    auto val = pre_delim(line, ":");
    trim_whitespace(val);
    trim_delim(val, "\"");
    return val;
}

template<val_t T>
void operator<<(pstream<T> const& ps, pset s){

    // Extract Line (w. Shift Pointer)

    auto line = get_line(s.t);
    trim_line(line, s.ind);

    // Extract Key, Value

    auto key = get_key(line);
    auto val = get_val(line);
    std::cout<<s.ind<<key<<val<<"\n";

    // Validate, Parse

    if(ps.t == NULL){
        // can't parse value
        return;
    }

    if(key != "" && key != s.key){
        throw parse_exception("unexpected key");
    }

   parse_val(ps.t->value, val);

}

template<arr_t T>
void operator<<(pstream<T> const& ps, pset s){

    if(s.key != NULL){

        // Extract Line (w. Shift Pointer)

        auto line = get_line(s.t);
        trim_line(line, s.ind);

        // Extract Key, Value

        auto key = get_key(line);
        auto val = get_val(line);
        std::cout<<s.ind<<key<<val<<"\n";
 
        // Validate, Parse

        if(key != "" && key != s.key)
            throw parse_exception("unexpected key");

        if(val != "")
            throw parse_exception("unexpected value");

        // Update Subsequent Expected Indentation State

        for(auto& st: s.ind.state)
            st = TAB;
        s.ind = s.ind + TAB;

    }

    ps.t->for_refs([&](auto&& ref){
        ref.node.impl << parse << pset{s.ind + DASH, NULL, s.t};
        for(auto& st: s.ind.state)
            st = TAB;
    });

}

template<obj_t T>
void operator<<(pstream<T> const& ps, pset s){

    if(s.key != NULL){

        // Extract Line (w. Shift Pointer)

        auto line = get_line(s.t);
        trim_prefix(line, s.ind.to_string());

        // Extract Key, Value

        auto key = get_key(line);
        auto val = get_val(line);
        std::cout<<s.ind<<key<<val<<"\n";

        // Validate, Parse

        if(key != "" && key != s.key)
            throw parse_exception("unexpected key");

        if(val != ""){
            throw parse_exception("unexpected value");
        }

        // Update Subsequent Expected Indentation State

        for(auto& st: s.ind.state)
            st = TAB;
        s.ind = s.ind + TAB;

    }

    ps.t->for_refs([&](auto&& ref){
        ref.node.impl << parse << pset{s.ind, ref.key, s.t};
         for(auto& st: s.ind.state)
            st = TAB;
    });

}

}   // end of namespace yaml
}   // end of namespace ctom

#endif