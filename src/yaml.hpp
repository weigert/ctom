#ifndef CTOM_YAML
#define CTOM_YAML

#include "ctom.hpp"
#include <string>
#include <iostream>

/*
    The new task is: write a yaml parser
    effectively, we know what we can expect right?

    because we have the object model, we should be able to parse it directly.
    I should therefore have a yaml marshal and yaml unmarshal method.


    I have:

    (key, val)
    (key, arr)
    (key, obj)

    note that keys are always contained by objects.

    Then I have:

    (ind, val)
    (ind, arr)
    (ind, obj)

    and inds are always contained by arrays.

    Then, for arrays, when every they point to something then
    the beginning of the element should be on the same line as the index.

    otherwise, indenting behavior is just as normal.


*/

namespace ctom {
namespace yaml {

// YAML Marshal

// Forward Declaration

template<val_t T>
void marshal(T&, std::string prefix = "");
template<arr_t T>
void marshal(T&, std::string prefix = "");
template<obj_t T>
void marshal(T&, std::string prefix = "");

// Implementation

template<val_t T>
void marshal(T& val, std::string prefix){
    std::cout<<val.value;
}

template<arr_t T>
void marshal(T& arr, std::string prefix){

    int n = 0;
    arr.for_refs([prefix, &n](auto&& ref){

        if(ref.node.type == "arr"){
            std::cout<<prefix;
            std::cout<<"- ";
            std::cout<<"\n";
            if(ref.node.impl != NULL)
                ctom::yaml::marshal(*ref.node.impl, prefix+"  ");
        } 

        if(ref.node.type == "obj"){
            std::cout<<prefix;
            std::cout<<"- ";
            std::cout<<"\n";
            if(ref.node.impl != NULL)
                ctom::yaml::marshal(*ref.node.impl, prefix+"  ");
        }

        if(ref.node.type == "val"){
            std::cout<<prefix;
            std::cout<<"- ";
            if(ref.node.impl != NULL)
                ctom::yaml::marshal(*ref.node.impl, prefix+"  ");
            std::cout<<"\n";
        }

        n++;
    });

}

template<obj_t T>
void marshal(T& obj, std::string prefix){

    // Iterate over Object References
  
    obj.for_refs([prefix](auto&& ref){

        if(ref.node.type == "arr"){
            std::cout<<prefix;
            std::cout<<ref.key<<": ";
            std::cout<<"\n";
            if(ref.node.impl != NULL)
                ctom::yaml::marshal(*ref.node.impl, prefix+"  ");
        }

        if(ref.node.type == "obj"){
            std::cout<<prefix;
            std::cout<<ref.key<<": ";
            std::cout<<"\n";
            if(ref.node.impl != NULL)
                ctom::yaml::marshal(*ref.node.impl, prefix+"  ");
        }

        if(ref.node.type == "val"){
            std::cout<<prefix;
            std::cout<<ref.key<<": ";
            if(ref.node.impl != NULL)
                ctom::yaml::marshal(*ref.node.impl, prefix+"  ");
            std::cout<<"\n";
        }

    });

}

}   // end of namespace yaml
}   // end of namespace ctom

#endif