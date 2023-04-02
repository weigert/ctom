#ifndef CTOM
#define CTOM

#include <iostream>
#include <type_traits>
#include <tuple>
#include <vector>
#include <initializer_list>
#include "key.hpp"
#include "util.hpp"
#include <stdio.h>
#include <string.h>

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

template<key_type ref, ctom::node_type T>
struct ref_node: ctom::ref_base{
  T node;
};

template<size_t ind, ctom::node_type T>
struct ind_node: ctom::ind_base{
  T node;
};

// Node-Implementation Forward Declarations

template<typename T> struct val_impl;
template<ctom::ind_type... T> struct arr_impl;
template<ctom::ref_type... T> struct obj_impl;

// Specific Node-Type Aliases

template<key_type ref, val_type T> using ref_val = ref_node<ref, ctom::node_val<T>>;
template<key_type ref, arr_type T> using ref_arr = ref_node<ref, ctom::node_arr<T>>;
template<key_type ref, obj_type T> using ref_obj = ref_node<ref, ctom::node_obj<T>>;

template<constexpr_string ref, typename T> using val = ctom::ref_val<constexpr_key<ref>, ctom::val_impl<T>>;
template<constexpr_string ref, arr_type T> using arr = ctom::ref_arr<constexpr_key<ref>, T>;
template<constexpr_string ref, obj_type T> using obj = ctom::ref_obj<constexpr_key<ref>, T>;

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

template<typename A, typename B>
struct is_ref_of {
  static constexpr bool value = false;
};

template <
  key_type key,
  template<typename, typename> typename ref, key_type refKey, typename T
> struct is_ref_of<key, ref<refKey, T>>{
  static constexpr bool value = is_same_key<key, refKey>::value;
};

template<size_t A, typename B>
struct is_ind_of {
  static constexpr bool value = false;
};

template <
  size_t ind,
  template<size_t, typename> typename ref, size_t refInd, typename T
> struct is_ind_of<ind, ref<refInd, T>>{
  static constexpr bool value = (ind == refInd);
};




template <key_type T, typename... Ts>
struct is_ref_contained;

template <key_type T>
struct is_ref_contained<T> {
  static constexpr bool value = false;
};

template <key_type A, typename B, typename... Ts>
struct is_ref_contained<A, B, Ts...>{
  static constexpr bool value = is_ref_of<A, B>::value
    || is_ref_contained<A, Ts...>::value;
};

template <size_t T, typename... Ts>
struct is_ind_contained;

template <size_t T>
struct is_ind_contained<T> {
  static constexpr bool value = false;
};

template <size_t A, typename B, typename... Ts>
struct is_ind_contained<A, B, Ts...>{
  static constexpr bool value = is_ind_of<A, B>::value
    || is_ind_contained<A, Ts...>::value;
};







template <size_t N, key_type T, typename... Ts>
struct ref_index {
  static constexpr size_t value = N;
};

template <size_t N, key_type T>
struct ref_index<N, T>{
    static constexpr size_t value = N;
};

template <size_t N, key_type A, ref_type B, ref_type... Ts>
requires(!is_ref_of<A, B>::value)
struct ref_index<N, A, B, Ts...> {
    static constexpr size_t value = ref_index<N + 1, A, Ts...>::value;
};

template <size_t N, size_t T, typename... Ts>
struct ind_index {
  static constexpr size_t value = N;
};

template <size_t N, size_t T>
struct ind_index<N, T>{
    static constexpr size_t value = N;
};

template <size_t N, size_t A, ref_type B, ref_type... Ts>
requires(!is_ind_of<A, B>::value)
struct ind_index<N, A, B, Ts...> {
    static constexpr size_t value = ind_index<N + 1, A, Ts...>::value;
};

template<typename A, typename B>
struct is_same_pair {
  static constexpr bool value = false;
};

template <
  template<typename, typename> typename refA, typename keyA, typename A,
  template<typename, typename> typename refB, typename keyB, typename B
> struct is_same_pair<refA<keyA, A>, refB<keyB, B>>{
  static constexpr bool value = is_same_key<keyA, keyB>::value;
};

template <
  template<size_t, typename> typename indA, size_t numA, typename A,
  template<size_t, typename> typename indB, size_t numB, typename B
> struct is_same_pair<indA<numA, A>, indB<numB, B>>{
  static constexpr bool value = (numA == numB);
};

//

template <typename... Ts>
struct is_pair_contained;

template <typename T>
struct is_pair_contained<T> {
  static constexpr bool value = false;
};

template <typename A, typename B, typename... Ts>
struct is_pair_contained<A, B, Ts...>{
  static constexpr bool value = is_same_pair<A, B>::value
    || is_pair_contained<A, Ts...>::value;
};

//

template <typename... Ts>
struct is_pair_unique;

template <>
struct is_pair_unique<> {
    static constexpr bool value = true;
};

template <typename T, typename... Ts>
struct is_pair_unique<T, Ts...>{
  static constexpr bool value = !is_pair_contained<T, Ts...>::value && is_pair_unique<Ts...>::value;
};

//

template <size_t N, typename T, typename... Ts>
struct pair_index {
  static constexpr size_t value = N;
};

template <size_t N, typename T>
struct pair_index<N, T>{
    static constexpr size_t value = N;
};

template <size_t N, ref_type A, ref_type B, ref_type... Ts>
requires(!is_same_pair<A, B>::value)
struct pair_index<N, A, B, Ts...> {
    static constexpr size_t value = pair_index<N + 1, A, Ts...>::value;
};

template <size_t N, ind_type A, ind_type B, ind_type... Ts>
requires(!is_same_pair<A, B>::value)
struct pair_index<N, A, B, Ts...> {
    static constexpr size_t value = pair_index<N + 1, A, Ts...>::value;
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

  static_assert(is_pair_unique<inds...>::value, "ind parameter indices for arr are not unique");

  std::tuple<inds...> nodes;
  static constexpr size_t size = std::tuple_size<std::tuple<inds...>>::value;

  template<ind_type... sinds>
  using ext = obj_impl<inds..., sinds...>;

  template<ind_type ind> struct index {
    static constexpr size_t value = pair_index<0, ind, inds...>::value;
  };

  template<ind_type ind>
  auto& get() {
    static_assert(is_pair_contained<ind, inds...>::value, "index is out of bounds");
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

//  static_assert(is_pair_unique<refs...>::value, "ref parameter keys for obj are not unique");

  std::tuple<refs...> nodes;
  static constexpr size_t size = std::tuple_size<std::tuple<refs...>>::value;

  // Extension

  template<ref_type... srefs>
  using ext = obj_impl<refs..., srefs...>;

  // Indexing Methods for Both!

  template<key_type ref> struct index {
    static constexpr size_t value = ctom::ref_index<0, ref, refs...>::value;
  };

  template<ref_type ref> struct ref_index {
    static constexpr size_t value = pair_index<0, ref, refs...>::value;
  };

  template<ref_type ref>
  auto& get() {
    static_assert(is_pair_contained<ref, refs...>::value, "ref not found");
    return std::get<ref_index<ref>::value>(nodes);
  }



  struct for_node {
    template<typename F>
    static constexpr void iter(F&& f){
      for_types<refs...>(f);
    }
  };


  template<constexpr_string ref>
  constexpr auto& get() {
    static_assert(is_ref_contained<constexpr_key<ref>, refs...>::value, "ref not found");
    return std::get<index<constexpr_key<ref>>::value>(nodes).node.val.value;
  }



  template<key_type ref>
  constexpr auto& get() {
    static_assert(is_ref_contained<ref, refs...>::value, "ref not found");
    return std::get<index<ref>::value>(nodes).node.val.value;
  }


  template<typename T>
  auto& operator[](T t){
    return get<T>();
  };













  // Instance Value Assignment

  template<constexpr_string key, typename T>
  auto& val(const T& t){
    auto& ref = get<ctom::ref_val<constexpr_key<key>, val_impl<T>>>();
    static_assert(std::is_same<val_impl<T>, decltype(ref.node.val)>::value, "can't assign val key to improper type");
    ref.node.val.value = std::move(t);
    return ref.node.val.value;
  }

  template<constexpr_string key, typename T>
  auto obj(const T& t){
    auto& ref = get<ref_obj<constexpr_key<key>, T>>();
    static_assert(is_derived<T, decltype(ref.node.obj)>::value, "can't assign obj key to non-derived type");
    ref.node.obj = t;
    return std::move(t);
  }

  template<constexpr_string key, typename T>
  auto arr(const std::initializer_list<T>& t){
    auto& ref = get<ref_arr<constexpr_key<key>, T>>();
    static_assert(is_derived<T, decltype(ref.node.arr)>::value, "can't assign arr key to non-derived type");
    ref.node.arr = t;
    return std::move(t);
  }
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

template<key_type ref, val_type T>
struct printer<ref_node<ref, node_val<T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<node_val<T>::type<<": ";
    std::cout<<ref::key<<"\n";
  }
};

template<key_type ref, arr_type T>
struct printer<ref_node<ref, node_arr<T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<node_arr<T>::type<<": ";
    std::cout<<ref::key<<"\n";
    T::for_node::iter([&]<typename N>(){
      printer<N>::print(shift+1);
    });
  }
};

template<key_type ref, obj_type T>
struct printer<ref_node<ref, node_obj<T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<node_obj<T>::type<<": ";
    std::cout<<ref::key<<"\n";
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

// Instance-Based Marshalling

// Reference

template<key_type ref, val_type T>
void print(ref_val<ref, T>& ref_val, size_t shift = 0){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<node_val<T>::type<<": ";
  std::cout<<ref::key<<" = ";
  std::cout<<ref_val.node.val.value<<"\n";
}

template<size_t ind, val_type T>
void print(ind_val<ind, T>& ind_val, size_t shift = 0){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<node_val<T>::type<<": ";
  std::cout<<ind<<" = ";
  std::cout<<ind_val.node.val.value<<"\n";
}



template<key_type ref, arr_type T>
void print(ref_arr<ref, T>& ref_arr, size_t shift = 0){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<node_arr<T>::type<<": ";
  std::cout<<ref::key<<" = ["<<"\n";
  std::apply([&](auto&&... args){
    (ctom::print(args, shift+1), ...);
  }, ref_arr.node.arr.nodes);
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<"]"<<std::endl;
}

template<key_type ref, obj_type T>
void print(ref_obj<ref, T>& ref_obj, size_t shift = 0){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<node_obj<T>::type<<": ";
  std::cout<<ref::key<<"\n";
  std::apply([&](auto&&... args){
    (ctom::print(args, shift+1), ...);
  }, ref_obj.node.obj.nodes);
}

// Index

template<size_t ind, arr_type T>
void print(ind_arr<ind, T>& ind_arr, size_t shift = 0){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<node_arr<T>::type<<": ";
  std::cout<<ind<<" = ["<<"\n";
  std::apply([&](auto&&... args){
    (ctom::print(args, shift+1), ...);
  }, ind_arr.node.arr.nodes);
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<"]"<<std::endl;
}

template<size_t ind, obj_type T>
void print(ind_obj<ind, T>& ind_obj, size_t shift = 0){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<node_obj<T>::type<<": ";
  std::cout<<ind<<"\n";
  std::apply([&](auto&&... args){
    (ctom::print(args, shift+1), ...);
  }, ind_obj.node.obj.nodes);
}

// Entry-Point

template<obj_type T>
void print(T& obj){
  std::apply([](auto&&... args){
    (ctom::print(args), ...);
  }, obj.nodes);
}

}

#endif
