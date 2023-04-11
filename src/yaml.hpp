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
*/

namespace ctom {
namespace yaml {

// Marshal

template<ctom::key_t IK, val_t T>
void marshal(ref_impl<IK, node_impl<T>>&, std::string prefix = "");
template<ctom::key_t IK, arr_t T>
void marshal(ref_impl<IK, node_impl<T>>&, std::string prefix = "");
template<ctom::key_t IK, obj_t T>
void marshal(ref_impl<IK, node_impl<T>>&, std::string prefix = "");

template<ctom::ind_t IK, val_t T>
void marshal(ref_impl<IK, node_impl<T>>&, std::string prefix = "");
template<ctom::ind_t IK, arr_t T>
void marshal(ref_impl<IK, node_impl<T>>&, std::string prefix = "");
template<ctom::ind_t IK, obj_t T>
void marshal(ref_impl<IK, node_impl<T>>&, std::string prefix = "");

// Entry-Point

template<ctom::obj_t T>
void marshal(T& obj, std::ostream& os = std::cout){
    std::apply([&](auto&&... args){
        (ctom::yaml::marshal(args), ...);
    }, obj.nodes);
}

// Ref Marhshal

template<ctom::key_t IK, val_t T>
void marshal(ref_impl<IK, node_impl<T>>& ref, std::string prefix){
    std::cout<<prefix<<IK::val<<": ";
    if(ref.node.impl != NULL)
    std::cout<<ref.node.impl->value;
    std::cout<<"\n";
}

template<ctom::key_t IK, arr_t T>
void marshal(ref_impl<IK, node_impl<T>>& ref, std::string prefix){
    std::cout<<prefix<<IK::val<<":\n";
    if(ref.node.impl != NULL)
    std::apply([&](auto&&... args){
        (ctom::yaml::marshal(args, prefix + "  "), ...);
    }, ref.node.impl->nodes);
}

template<ctom::key_t IK, obj_t T>
void marshal(ref_impl<IK, node_impl<T>>& ref, std::string prefix){
    std::cout<<prefix<<IK::val<<":\n";
    if(ref.node.impl != NULL)
    std::apply([&](auto&&... args){
        (ctom::yaml::marshal(args, prefix + "  "), ...);
    }, ref.node.impl->nodes);
}

template<ctom::ind_t IK, val_t T>
void marshal(ref_impl<IK, node_impl<T>>& ref, std::string prefix){
    std::cout<<prefix<<"- ";
    if(ref.node.impl != NULL)
    std::cout<<ref.node.impl->value;
    std::cout<<"\n";
}

template<ctom::ind_t IK, arr_t T>
void marshal(ref_impl<IK, node_impl<T>>& ref, std::string prefix){
    std::cout<<prefix<<"- ";
    if(ref.node.impl != NULL)
    std::apply([&](auto&&... args){
        ([&](){
          //  std::cout<<prefix<<"  -";
            ctom::yaml::marshal(args, prefix + "  ");
        }(), ...);
    }, ref.node.impl->nodes);
}

template<ctom::ind_t IK, obj_t T>
void marshal(ref_impl<IK, node_impl<T>>& ref, std::string prefix){
    std::cout<<prefix<<"- ";
    if(ref.node.impl != NULL)
    std::apply([&](auto&&... args){
        ([&](){
        //    std::cout<<args.val;//<<std::endl;
            ctom::yaml::marshal(args, prefix + "  ");
        }(), ...);
    }, ref.node.impl->nodes);
}

}   // end of namespace yaml
}   // end of namespace ctom

#endif