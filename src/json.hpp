#ifndef CTOM_JSON
#define CTOM_JSON

#include "ctom.hpp"

#include <string>
#include <string_view>
#include <vector>
#include <charconv>

namespace ctom {
namespace json {

struct ostream_json: ctom::ostream_base{} static emit;
struct istream_json: ctom::istream_base{} static parse;

using ostream = ctom::ostream<ostream_json>;
using istream = ctom::istream<istream_json>;

ostream operator<<(std::ostream& os, ostream_json&) {
    return ostream(os);
}

istream operator>>(std::istream& is, istream_json&) {
    return istream(is);
}

/*
================================================================================
                            JSON Model Co-State
================================================================================
*/

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

// Reference State

template<typename T>
struct set {
    indent ind;
    const char* key;
    T* t;
    bool last = true;
};

template<typename T>
ostream operator<<(ostream const& os, T& type){
    typename rule<T>::type ref(type);
    return os << set{{}, NULL, &ref};
}

/*
================================================================================
                            JSON Marshal Implementation
================================================================================
*/

template<val_t T>
ostream operator<<(ostream const& os, set<T> s){

    os.os << s.ind.to_string();
    
    if(s.key != NULL)
        os.os << "\"" << s.key << "\": ";
    
    if(s.t != NULL) os.os << "\"" << *s.t->value << "\"";
    else os.os << "\"\"";

    if(!s.last) os.os << ",";
    os.os << "\n";

    return os;
}

template<arr_t T>
ostream operator<<(ostream const& os, set<T> s){

    os.os << s.ind.to_string();

    if(s.key != NULL)
        os.os << "\"" << s.key << "\": ";
    os.os << "[\n";

    int n = 0;
    s.t->for_refs([&](auto&& ref){
        os << set{s.ind+TAB, NULL, ref.node.impl, (n == s.t->size-1)};
        n++;
    });

    os.os << s.ind.to_string() << "]";
    if(!s.last) os.os << ",";
    os.os << "\n";

    return os;
}

template<obj_t T>
ostream operator<<(ostream const& os, set<T> s){

    os.os << s.ind.to_string();
    if(s.key != NULL)
        os.os << "\"" << s.key << "\": ";
    os.os << "{\n";

    int n = 0;
    s.t->for_refs([&](auto&& ref){
        os << set{s.ind+TAB, ref.key, ref.node.impl, (n == s.t->size-1)};
        n++;
    });

    os.os << s.ind.to_string() << "}";
    if(!s.last) os.os << ",";
    os.os << "\n";

    return os;

}

}   // end of namespace yaml
}   // end of namespace ctom

#endif