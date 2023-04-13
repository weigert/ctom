#ifndef CTOM_YAML
#define CTOM_YAML

#include "ctom.hpp"
#include <string>
#include <iostream>
#include <vector>

namespace ctom {
namespace yaml {

/*

we probably need to pack out bigger guns.
as in, use data-types to allow for merging
of specific indenting specifiers.

this should also let us later match them...
well see.

*/

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

std::ostream& operator<<(std::ostream& os, indent& i){
    for(auto& s: i.state){
        if(s == TAB) os << "  ";
        if(s == DASH) os << "- ";
    }
    return os;
}

// YAML Marshal

// Forward Declaration

template<val_t T>
void marshal(indent = {}, const char* key = "", T* = NULL);
template<arr_t T>
void marshal(indent = {}, const char* key = "", T* = NULL);
template<obj_t T>
void marshal(indent = {}, const char* key = "", T* = NULL);

template<typename T>
void marshal(T& t){
    marshal<T>({}, NULL, &t);
}

// Implementation

template<val_t T>
void marshal(indent ind, const char* key, T* val){
    std::cout<<ind;
    if(key != NULL){
        std::cout<<key<<": ";
    }
    if(val != NULL) std::cout<<val->value;
    std::cout<<"\n";
}

template<arr_t T>
void marshal(indent ind, const char* key, T* arr){

    if(key != NULL){
        std::cout<<ind<<key<<":\n";
        ind = ind + TAB;
        for(auto& s: ind.state)
            s = TAB;
    }

    arr->for_refs([&](auto&& ref){
        ctom::yaml::marshal(ind + DASH, NULL, ref.node.impl);
        for(auto& s: ind.state)
            s = TAB;
    });

}

template<obj_t T>
void marshal(indent ind, const char* key, T* obj){

    if(key != NULL){
        std::cout<<ind<<key<<":\n";
        for(auto& s: ind.state)
            s = TAB;
        ind = ind + TAB;
    }

    obj->for_refs([&](auto&& ref){
        ctom::yaml::marshal(ind, ref.key, ref.node.impl);
        for(auto& s: ind.state)
            s = TAB;
    });

}

}   // end of namespace yaml
}   // end of namespace ctom

#endif