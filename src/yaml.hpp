#ifndef CTOM_YAML
#define CTOM_YAML

#include "ctom.hpp"

#include <string>
#include <string_view>
#include <vector>
#include <charconv>

namespace ctom {
namespace yaml {

struct ostream_yaml: ctom::ostream_base{} static emit;
struct istream_yaml: ctom::istream_base{} static parse;

using ostream = ctom::ostream<ostream_yaml>;
using istream = ctom::istream<istream_yaml>;

ostream operator<<(std::ostream& os, ostream_yaml&) {
    return ostream(os);
}

istream operator>>(std::istream& is, istream_yaml&) {
    return istream(is);
}

/*
================================================================================
                            YAML Model Co-State
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
};

template<typename T>
ostream operator<<(ostream const& os, T& type){
    typename rule<T>::type ref(type);
    return os << set{{}, NULL, &ref};
}

template<typename T>
void operator>>(istream is, T& type){
    typename rule<T>::type ref(type);
    is >> set{{}, NULL, &ref};
}

/*
================================================================================
                        YAML Marshal Implementation
================================================================================
*/

// Marshal Implementation

template<val_t T>
ostream operator<<(ostream const& os, set<T> s){

    os.os << s.ind.to_string();

    if(s.key != NULL)
        os.os << s.key << ": ";
    
    if(s.t != NULL) 
        os.os << *(s.t->value);
    else os.os << "null";
    
    os.os << "\n";
    return os;

}

template<arr_t T>
ostream operator<<(ostream const& os, set<T> s){

    if(s.key != NULL){

        os.os << s.ind.to_string() << s.key << ":";
        if(s.t == NULL) os.os << " null";
        os.os << "\n";

        for(auto& st: s.ind.state)
            st = TAB;
        s.ind = s.ind + TAB;
    }

    if(s.t != NULL)
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

        os.os << s.ind.to_string() << s.key << ":";
        if(s.t == NULL) os.os << " null";
        os.os << "\n";

        for(auto& st: s.ind.state)
            st = TAB;
        s.ind = s.ind + TAB;
    }

    if(s.t != NULL)
    s.t->for_refs([&](auto&& ref){
        os << set{s.ind, ref.key, ref.node.impl};
        for(auto& st: s.ind.state)
            st = TAB;
    });

    return os;

}

/*
================================================================================
                        YAML Unmarshal Implementation
================================================================================
*/

// Base Trim Operations

std::string_view pre_delim(std::string_view sv, std::string_view delim){
    if(sv.find(delim) != std::string_view::npos)
        sv = sv.substr(0, sv.find(delim));
    return sv;
}

void trim_prefix(std::string_view& sv, std::string_view prefix){
    if(!sv.starts_with(prefix))
        throw parse_exception("failed to trim prefix");
    sv.remove_prefix(prefix.size());
}

void trim_delim(std::string_view& sv, std::string_view delim){
    if(sv.starts_with(delim) && sv.ends_with(delim)){
        sv.remove_prefix(delim.size());
        sv.remove_suffix(delim.size());
    }
}

void trim_whitespace(std::string_view& line){
    if(line.find_first_not_of(" \t") == std::string_view::npos)
        line.remove_prefix(line.size());
    if(line.find_first_not_of(" \t") != std::string_view::npos)
        line.remove_prefix(line.find_first_not_of(" \t"));
    if(line.find_last_not_of(" \t") != std::string_view::npos)
        line.remove_suffix(line.size()-line.find_last_not_of(" \t")-1);
}

// Value-Parser

template<typename T>
void parse_val(T& t, std::string_view v){
    auto val = std::from_chars(v.data(), v.data() + v.size(), t);
    if(val.ec == std::errc::invalid_argument)
        throw parse_exception("invalid argument");
}

template<>
void parse_val<char>(char& t, std::string_view v){
    if(v.size() != 1)
        throw parse_exception("invalid size for char");
    t = v[0];
}

template<>
void parse_val<std::string>(std::string& t, std::string_view v){
    t = v;
}

// Stream Base-Operations

std::string_view get_line(istream& ifs){
    while(!ifs.is.eof()){

        std::getline(ifs.is, ifs.cur);
        ifs.line++;

        auto view = pre_delim(ifs.cur, "#");
        if(view.find_first_not_of(" \t") != std::string_view::npos)
            return view;

    }
    throw exception(ifs.line, "unexpected eof");
}

std::string_view get_key(std::string_view line){
    if(line.find(":") == std::string_view::npos)
        return "";
    auto key = line.substr(0, line.find(":"));
    trim_whitespace(key);
    trim_delim(key, "\"");
    return key;
}

std::string_view get_val(std::string_view line){
    std::string_view val;
    if(line.find(":") == std::string_view::npos)
        val = line.substr(0, line.find(":"));
    else 
        val = line.substr(line.find(":")+1);
    trim_whitespace(val);
    trim_delim(val, "\"");
    return val;
}

template<val_t T>
void operator>>(istream& stream, set<T> s){

    // Extract Line (w. Shift Pointer)

    auto line = get_line(stream);

    try {
        trim_prefix(line, s.ind.to_string());
        if(line.find_first_not_of(" \t") != 0)
            throw parse_exception("overindented");
    } catch(parse_exception e){
        throw exception(stream.line, std::string("invalid indent: ") + e.what());
    }

    // Extract Key, Value; Validate

    auto key = get_key(line);
    auto val = get_val(line);

    if(s.key == NULL && key != "")
        throw exception(stream.line, std::string("invalid key: want null, have \"") + std::string(key) + "\n");

    if(s.key != NULL && key != s.key)
        throw exception(stream.line, std::string("invalid key: want \"")+std::string(s.key)+"\", have \""+std::string(key)+"\"");

    // Validate, Parse

    if(s.t != NULL && val != "")
    try {
        parse_val(*s.t->value, val);
    } catch(parse_exception e){
        throw exception(stream.line, std::string("failed to parse value: ") + e.what());
    }

}

template<arr_t T>
void operator>>(istream& stream, set<T> s){

    if(s.key != NULL){

        // Extract Line (w. Shift Pointer)

        auto line = get_line(stream);

        try {
            trim_prefix(line, s.ind.to_string());
            if(line.find_first_not_of(" \t") != 0)
                throw parse_exception("overindented");
        } catch(parse_exception e){
            throw exception(stream.line, std::string("invalid indent: ") + e.what());
        }

       // Extract Key, Value

        auto key = get_key(line);
        auto val = get_val(line);

        // Validate, Parse

        if(key != "" && key != s.key)
            throw exception(stream.line, std::string("invalid key: want \"")+std::string(s.key)+"\", have \""+std::string(key)+"\"");

        if(val != "")
            throw exception(stream.line, std::string("unexpected value"));

        // Update Subsequent Expected Indentation State

        for(auto& st: s.ind.state)
            st = TAB;
        s.ind = s.ind + TAB;

    }

    if(s.t != NULL)
    s.t->for_refs([&](auto&& ref){
        stream >> set{s.ind + DASH, NULL, ref.node.impl};
        for(auto& st: s.ind.state)
            st = TAB;
    });

}

template<obj_t T>
void operator>>(istream& stream, set<T> s){

    // Extract Line (w. Shift Pointer)

    if(s.key != NULL){

        auto line = get_line(stream);

        try {
            trim_prefix(line, s.ind.to_string());
            if(line.find_first_not_of(" \t") != 0)
                throw parse_exception("overindented");
        } catch(parse_exception e){
            throw exception(stream.line, std::string("invalid indent: ") + e.what());
        }

        // Extract Key, Value

        auto key = get_key(line);
        auto val = get_val(line);

        // Validate, Parse

        if(key != "" && key != s.key)
            throw exception(stream.line, std::string("invalid key: want \"")+std::string(s.key)+"\", have \""+std::string(key)+"\"");
        
        if(val != "")
            throw exception(stream.line, std::string("unexpected value"));

        // Update Subsequent Expected Indentation State

        for(auto& st: s.ind.state)
            st = TAB;
        s.ind = s.ind + TAB;

    }

    if(s.t != NULL)
    s.t->for_refs([&](auto&& ref){
        stream >> set{s.ind, ref.key, ref.node.impl};
        for(auto& st: s.ind.state)
            st = TAB;
    });

}

}   // end of namespace yaml
}   // end of namespace ctom

#endif