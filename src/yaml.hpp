#ifndef CTOM_YAML
#define CTOM_YAML

#include "ctom.hpp"
#include <string>
#include <iostream>

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

    arr.for_refs([prefix](auto&& ref){

        if(ref.node.type == "val"){
            std::cout<<prefix;
            std::cout<<"- ";
            if(ref.node.impl != NULL)
                ctom::yaml::marshal(*ref.node.impl, prefix+"  ");
            std::cout<<"\n";
        }

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

    });

}

template<obj_t T>
void marshal(T& obj, std::string prefix){

    // Iterate over Object References
  
    obj.for_refs([prefix](auto&& ref){

        if(ref.node.type == "val"){
            std::cout<<prefix;
            std::cout<<ref.key<<": ";
            if(ref.node.impl != NULL)
                ctom::yaml::marshal(*ref.node.impl, prefix+"  ");
            std::cout<<"\n";
        }

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

    });

}

}   // end of namespace yaml
}   // end of namespace ctom

#endif