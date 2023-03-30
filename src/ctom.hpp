#ifndef CTOM
#define CTOM

#include <iostream>
#include <type_traits>
#include <tuple>
#include <vector>
#include <initializer_list>
#include "key.hpp"
#include "util.hpp"

namespace ctom {

/*
================================================================================
                        Node-Types and Implementations
================================================================================
*/

struct node_base{};
struct val_base{};
struct arr_base{};
struct obj_base{};

template<typename T> concept node_type = std::derived_from<T, ctom::node_base>;
template<typename T> concept val_type = std::derived_from<T, ctom::val_base>;
template<typename T> concept arr_type = std::derived_from<T, ctom::arr_base>;
template<typename T> concept obj_type = std::derived_from<T, ctom::obj_base>;

template<ctom::val_type T>
struct node_val: node_base {
  static constexpr char const* type = "val";
  T val;
};

template<ctom::arr_type T>
struct node_arr: node_base {
  static constexpr char const* type = "arr";
  T arr;
};

template<ctom::obj_type T>
struct node_obj: node_base {
  static constexpr char const* type = "obj";
  T obj;
};

/*
================================================================================
                            References and Indices
================================================================================
*/

struct ref_base{};
struct ind_base{};

template<typename T> concept ref_type = std::derived_from<T, ctom::ref_base>;
template<typename T> concept ind_type = std::derived_from<T, ctom::ind_base>;

// References and Indices of Node Types

template<constexpr_string ref, ctom::node_type T>
struct ref_node: ctom::ref_base{};

template<size_t ind, ctom::node_type T>
struct ind_node: ctom::ind_base{};

// Node-Implementation Forward Declarations

template<typename T> struct val_impl;
template<ctom::ind_type... T> struct arr_impl;
template<ctom::ref_type... T> struct obj_impl;

// Specific Node-Type Aliases

template<constexpr_string ref, val_type T> using ref_val = ref_node<ref, ctom::node_val<T>>;
template<constexpr_string ref, arr_type T> using ref_arr = ref_node<ref, ctom::node_arr<T>>;
template<constexpr_string ref, obj_type T> using ref_obj = ref_node<ref, ctom::node_obj<T>>;

template<constexpr_string ref, typename T> using val = ctom::ref_val<ref, ctom::val_impl<T>>;
template<constexpr_string ref, arr_type T> using arr = ctom::ref_arr<ref, T>;
template<constexpr_string ref, obj_type T> using obj = ctom::ref_obj<ref, T>;

template<size_t ind, val_type T> using ind_val = ind_node<ind, ctom::node_val<T>>;
template<size_t ind, arr_type T> using ind_arr = ind_node<ind, ctom::node_arr<T>>;
template<size_t ind, obj_type T> using ind_obj = ind_node<ind, ctom::node_obj<T>>;

template<size_t ind, typename T> using _val = ctom::ind_val<ind, ctom::val_impl<T>>;
template<size_t ind, arr_type T> using _arr = ctom::ind_arr<ind, T>;
template<size_t ind, obj_type T> using _obj = ctom::ind_obj<ind, T>;

/*
================================================================================
                          Helper Constexpr Structs
================================================================================
*/

//

template<ref_type refA, ref_type refB>
struct is_same_ref {
  static constexpr bool value = false;
};

template <
  template<constexpr_string, typename> typename refA, constexpr_string keyA, typename A,
  template<constexpr_string, typename> typename refB, constexpr_string keyB, typename B
> struct is_same_ref<refA<keyA, A>, refB<keyB, B>>{
  static constexpr bool value = is_same_key<keyA, keyB>::value;
};

//

template <ref_type ref, ref_type... refs>
struct is_ref_contained;

template <ref_type ref>
struct is_ref_contained<ref> {
  static constexpr bool value = false;
};

template <ref_type refA, ref_type refB, ref_type... refs>
struct is_ref_contained<refA, refB, refs...>{
  static constexpr bool value = is_same_ref<refA, refB>::value
    || is_ref_contained<refA, refs...>::value;
};

//

template <size_t N, ref_type ref, ref_type... refs>
struct ref_index {
    static constexpr size_t value = N;
};

template <size_t N, ref_type ref>
struct ref_index<N, ref>{
    static constexpr size_t value = N;
};

template <size_t N, ref_type refA, ref_type refB, ref_type... refs>
requires(!is_same_ref<refA, refB>::value)
struct ref_index<N, refA, refB, refs...> {
    static constexpr size_t value = ref_index<N + 1, refA, refs...>::value;
};

//

//

template<ind_type indA, ref_type indB>
struct is_same_ind {
  static constexpr bool value = false;
};

template <
  template<size_t, typename> typename indA, size_t numA, typename A,
  template<size_t, typename> typename indB, size_t numB, typename B
> struct is_same_ref<indA<numA, A>, indB<numB, B>>{
  static constexpr bool value = (numA == numB);
};

//

template <ind_type ind, ind_type... inds>
struct is_ind_contained;

template <ind_type ind>
struct is_ind_contained<ind> {
  static constexpr bool value = false;
};

template <ind_type indA, ind_type indB, ind_type... inds>
struct is_ind_contained<indA, indB, inds...>{
  static constexpr bool value = is_same_ind<indA, indB>::value
    || is_ind_contained<indA, inds...>::value;
};

//

template <size_t N, ind_type ind, ind_type... inds>
struct ind_index {
    static constexpr size_t value = N;
};

template <size_t N, ind_type ind>
struct ind_index<N, ind>{
    static constexpr size_t value = N;
};

template <size_t N, ind_type indA, ind_type indB, ind_type... inds>
requires(!is_same_ind<indA, indB>::value)
struct ind_index<N, indA, indB, inds...> {
    static constexpr size_t value = ind_index<N + 1, indA, inds...>::value;
};

/*
================================================================================

================================================================================
*/

template<typename T>
struct val_impl: val_base {
  T value;
};

template<ind_type... inds>
struct arr_impl: arr_base {

  std::tuple<inds...> nodes;
  static constexpr size_t size = std::tuple_size<std::tuple<inds...>>::value;

  template<ind_type... sinds>
  using ext = obj_impl<inds..., sinds...>;

  template<ind_type ind> struct index {
    static constexpr size_t value = ind_index<0, ind, inds...>::value;
  };

  template<ind_type ind>
  auto& get() {
    static_assert(index<ind>::value < size, "index is out of bounds");
    return std::get<index<ind>::value>(nodes);
  }

  struct for_node {
    template<typename F>
    static constexpr void iter(F&& f){
      for_types<inds...>(f);
    }
  };

};

template<ref_type... refs>
struct obj_impl: obj_base {

  //static_assert(is_same<ref_type, refs...>::value, "template parameters for obj are not of type ref");
  //static_assert(is_ref_key_unique<refs...>::value, "ref parameter keys for obj are not unique");

  std::tuple<refs...> nodes;
  static constexpr size_t size = std::tuple_size<std::tuple<refs...>>::value;

  // Extension

  template<ref_type... srefs>
  using ext = obj_impl<refs..., srefs...>;

  // Indexing Methods for Both!

  template<ref_type ref> struct index {
    static constexpr size_t value = ref_index<0, ref, refs...>::value;
  };

  template<ref_type ref>
  auto& get() {
    static_assert(index<ref>::value < size, "index is out of bounds");
    return std::get<index<ref>::value>(nodes);
  }

  struct for_node {
    template<typename F>
    static constexpr void iter(F&& f){
      for_types<refs...>(f);
    }
  };

/*
  // Instance Value Assignment

  template<constexpr_string ref, typename T>
  auto val(const T& t){
    static_assert(is_val<T>, "type is not a value type");
    static_assert(is_ref_key_contained<ctom::val<ref, T>, refs...>::value, "key does not exist in ctom::obj_impl");
    static_assert(is_contained<ctom::val<ref, T>, refs...>::value, "can't assign val key to improper type");
    auto& node = get<ctom::val<ref, T>>();
    node.val.value = t;
    return std::move(t);
  }

  template<constexpr_string ref, typename T>
  auto obj(const T& t){
    static_assert(obj_type<T>, "type is not a derived type of ctom::obj_impl");
    static_assert(is_ref_key_contained<ref_obj<ref, T>, refs...>::value, "key does not exist");
    static_assert(is_derived_ref_contained<ref_obj<ref, T>, refs...>::value, "can't assign obj key to non-derived type");
    auto& node = get<ref_obj<ref, T>>();
    node.obj = t;
    return std::move(t);
  }

  template<constexpr_string ref, typename T>
  auto arr(const std::initializer_list<T>& t){
    static_assert(is_arr<T>, "type is not a derived type of ctom::arr");
    static_assert(is_ref_key_contained<ctom::arr<ref, T>, refs...>::value, "key does not exist");
    static_assert(is_derived_ref_contained<ctom::arr<ref, T>, refs...>::value, "can't assign arr key to non-derived array type");
    auto& node = get<ctom::arr<ref, T>>();
    //note: the node doesn't need to be populated by the actual
    // values, but rather references to the value
    // it requires its own indexing structure, etc..
    //
    node.arr.values = t;
    return std::move(t);
  }


  // Object Extension


*/

};


/*
================================================================================
                          Marshalling / Unmarshalling
================================================================================
*/

template<typename T>
struct printer {
  static void print(size_t shift = 0);
};

// Ref-Type Printing

template<constexpr_string ref, val_type T>
struct printer<ref_node<ref, node_val<T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<node_val<T>::type<<": ";
    std::cout<<ref<<"\n";
  }
};

template<constexpr_string ref, arr_type T>
struct printer<ref_node<ref, node_arr<T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<node_arr<T>::type<<": ";
    std::cout<<ref<<"\n";
    T::for_node::iter([&]<typename N>(){
      printer<N>::print(shift+1);
    });
  }
};

template<constexpr_string ref, obj_type T>
struct printer<ref_node<ref, node_obj<T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<node_obj<T>::type<<": ";
    std::cout<<ref<<"\n";
    T::for_node::iter([&]<typename N>(){
      printer<N>::print(shift+1);
    });
  }
};

// Ind-Type Printing

template<size_t ind, val_type T>
struct printer<ind_node<ind, node_val<T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<node_val<T>::type<<": ";
    std::cout<<ind<<"\n";
  }
};

template<size_t ind, arr_type T>
struct printer<ind_node<ind, node_arr<T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<node_arr<T>::type<<": ";
    std::cout<<ind<<"\n";
    T::for_node::iter([&]<typename N>(){
      printer<N>::print(shift+1);
    });
  }
};

template<size_t ind, obj_type T>
struct printer<ind_node<ind, node_obj<T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<node_obj<T>::type<<": ";
    std::cout<<ind<<"\n";
    T::for_node::iter([&]<typename N>(){
      printer<N>::print(shift+1);
    });
  }
};

// Print an Object-Type Directly

template<obj_type T>
struct printer<T>{
  static void print(size_t shift = 0){
    T::for_node::iter([&]<typename N>(){
      printer<N>::print(shift);
    });
  }
};

// Entry-Point

template<typename T>
void print(){
  printer<T>::print();
}

}

#endif
