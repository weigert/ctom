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

struct node_t{};
struct val_t{ static constexpr char const* type = "val"; };
struct arr_t{ static constexpr char const* type = "arr"; };
struct obj_t{ static constexpr char const* type = "obj"; };

template<typename T> concept node_t = std::derived_from<T, ctom::node_t>;
template<typename T> concept val_t = std::derived_from<T, ctom::val_t>;
template<typename T> concept arr_t = std::derived_from<T, ctom::arr_t>;
template<typename T> concept obj_t = std::derived_from<T, ctom::obj_t>;

template<typename T>
struct node_impl: node_t {
  T* impl = NULL;
  ~node_impl(){
    if(impl != NULL) delete impl;
  }
};

// Static Check

template<node_t NA, node_t NB>
struct is_derived_node {
  static constexpr bool value = false;
};

template <
  template<typename> typename NA, val_t VA,
  template<typename> typename NB, val_t VB
> struct is_derived_node<NA<VA>, NB<VB>>{
  static constexpr bool value = std::is_base_of<VB, VA>::value;
};

template <
  template<typename> typename NA, arr_t VA,
  template<typename> typename NB, arr_t VB
> struct is_derived_node<NA<VA>, NB<VB>>{
  static constexpr bool value = std::is_base_of<VB, VA>::value;
};

template <
  template<typename> typename NA, obj_t VA,
  template<typename> typename NB, obj_t VB
> struct is_derived_node<NA<VA>, NB<VB>>{
  static constexpr bool value = std::is_base_of<VB, VA>::value;
};

/*
================================================================================
                      Key, Index and Reference Types
================================================================================
*/

struct ind_t{};
struct key_t{};
struct ref_t{};

template<typename T> concept ind_t = std::derived_from<T, ctom::ind_t>;
template<typename T> concept key_t = std::derived_from<T, ctom::key_t>;
template<typename T> concept ref_t = std::derived_from<T, ctom::ref_t>;
template<typename T> concept ind_key_t = ind_t<T> || key_t<T>;

template<size_t S>
struct ind_impl: ind_t {
  static constexpr auto val = S;
};

template<constexpr_string S>
struct key_impl: key_t {
  static constexpr auto val = S;
};





template<ind_key_t T, node_t N>
struct ref_impl: ref_t {
  N node;
};


// Specific Reference-SubType

template<typename T> struct is_ind_ref {
  static constexpr bool value = false;
};

template<template<typename, typename>typename R, ind_t I, node_t N>
struct is_ind_ref<R<I, N>>{
  static constexpr bool value = true;
};

template<typename T> concept ind_ref_t = ref_t<T> && is_ind_ref<T>::value;

template<typename T> struct is_key_ref {
  static constexpr bool value = false;
};

template<template<typename, typename>typename R, key_t I, node_t N>
struct is_key_ref<R<I, N>>{
  static constexpr bool value = true;
};

template<typename T> concept key_ref_t = ref_t<T> && is_key_ref<T>::value;




// I need to use user-defined class template automatic deduction

// THESE ALIASES CAN PROBABLY BE RENAMED!!!!


// Node-Implementation Forward Declarations

template<typename T> struct val_impl;
template<ind_ref_t... T> struct arr_impl;
template<key_ref_t... T> struct obj_impl;

// Node-Type Aliases

template<ind_key_t IK, val_t T> using ref_val = ref_impl<IK, node_impl<T>>;
template<ind_key_t IK, arr_t T> using ref_arr = ref_impl<IK, node_impl<T>>;
template<ind_key_t IK, obj_t T> using ref_obj = ref_impl<IK, node_impl<T>>;






template<constexpr_string ref, typename T> using val = ctom::ref_val<key_impl<ref>, ctom::val_impl<T>>;
template<size_t ind, typename T> using _val = ctom::ref_val<ind_impl<ind>, ctom::val_impl<T>>;

//template<ind_key_t IK, typename T> using val = ctom::ref_val<IK, ctom::val_impl<T>>;

template<constexpr_string ref, arr_t T> using arr = ctom::ref_arr<key_impl<ref>, T>;
template<constexpr_string ref, obj_t T> using obj = ctom::ref_obj<key_impl<ref>, T>;

template<size_t ind, arr_t T> using _arr = ctom::ref_arr<ind_impl<ind>, T>;
template<size_t ind, obj_t T> using _obj = ctom::ref_obj<ind_impl<ind>, T>;

/*
================================================================================
                          Helper Constexpr Structs
================================================================================
*/

// static check if ind_key_t is the key of a ref_t

template<typename T, ref_t R>
struct is_same_ref {
  static constexpr bool value = false;
};

template <ind_key_t IKA,
  template<typename, typename> typename R, ind_key_t IKB, node_t N
> struct is_same_ref<IKA, R<IKB, N>>{
  static constexpr bool value = std::is_same<IKA, IKB>::value;
};

template <
  template<typename, typename> typename RA, ind_key_t IKA, node_t NA,
  template<typename, typename> typename RB, ind_key_t IKB, node_t NB
> struct is_same_ref<RA<IKA, NA>, RB<IKB, NB>>{
  static constexpr bool value = std::is_same<IKA, IKB>::value;
};

// static check if ind_key_t is the key of any ref_t

template <typename T, ref_t... Rs>
struct is_ref_contained;

template <typename T>
struct is_ref_contained<T> {
  static constexpr bool value = false;
};

template <ind_key_t IK, ref_t R, ref_t... Rs>
struct is_ref_contained<IK, R, Rs...>{
  static constexpr bool value = is_same_ref<IK, R>::value
    || is_ref_contained<IK, Rs...>::value;
};

template <ref_t RA, ref_t RB, ref_t... Rs>
struct is_ref_contained<RA, RB, Rs...>{
  static constexpr bool value = is_same_ref<RA, RB>::value
    || is_ref_contained<RA, Rs...>::value;
};

// static index fetch for ind_key_t in set of ref_t

template <size_t N, typename T, ref_t... Rs>
struct index_refs {
  static constexpr size_t value = N;
};

template <size_t N, typename T>
struct index_refs<N, T>{
    static constexpr size_t value = N;
};

template <size_t N, ind_key_t IK, ref_t R, ref_t... Rs>
requires(!is_same_ref<IK, R>::value)
struct index_refs<N, IK, R, Rs...> {
    static constexpr size_t value = index_refs<N + 1, IK, Rs...>::value;
};

template <size_t N, ref_t RA, ref_t RB, ref_t... Rs>
requires(!is_same_ref<RA, RB>::value)
struct index_refs<N, RA, RB, Rs...> {
    static constexpr size_t value = index_refs<N + 1, RA, Rs...>::value;
};

//

template <ref_t... Rs>
struct is_ref_unique;

template <>
struct is_ref_unique<> {
    static constexpr bool value = true;
};

template <ref_t R, ref_t... Rs>
struct is_ref_unique<R, Rs...>{
  static constexpr bool value = !is_ref_contained<R, Rs...>::value && is_ref_unique<Rs...>::value;
};

/*
================================================================================

================================================================================
*/

template<typename T>
struct val_impl: val_t {
  T value;
  val_impl(){}
  val_impl(T&& t) noexcept {
		value = t;
	}
};

template<ind_ref_t... inds>
struct arr_impl: arr_t {

  static_assert(is_ref_unique<inds...>::value, "indices for arr are not unique");

  std::tuple<inds...> nodes;
  static constexpr size_t size = std::tuple_size<std::tuple<inds...>>::value;

  template<ind_ref_t... sinds>
  using ext = obj_impl<inds..., sinds...>;

  template<ctom::ind_t ind> struct index {
    static constexpr size_t value = ctom::index_refs<0, ind, inds...>::value;
  };

  template<ctom::ind_t ind>
  constexpr auto& get() {
    static_assert(is_ref_contained<ind, inds...>::value, "index is out of bounds");
    return std::get<index<ind>::value>(nodes);
  }

  template<size_t ind>
  constexpr auto& get() {
    return get<ind_impl<ind>>().node.impl->value;
  }

  template<ind_t ind>
  auto& operator[](ind){
    return *(get<ind>().node.impl);
  };

  struct for_node {
    template<typename F>
    static constexpr void iter(F&& f){
      for_types<inds...>(f);
    }
  };

  //

  template<size_t ind, typename T>
  auto& val(const T& t){
    auto& ref = get<ind_impl<ind>>();
  //  static_assert(std::is_same<val_impl<T>, decltype(ref.node.impl)>::value, "can't assign val key to improper type");
    auto i = new val_impl<T>();
    ref.node.impl = i;
    ref.node.impl->value = t;
    return ref.node.impl->value;
  }

  template<size_t ind, typename T>
  auto& obj(){
    auto& ref = get<ind_impl<ind>>();
    static_assert(is_derived_node<node_impl<T>, decltype(ref.node)>::value, "can't assign obj ind to non-derived type");
    auto t = new T();
    ref.node.impl = t;
    return *t;
  }

  template<size_t ind, typename T>
  auto& arr(){
    auto& ref = get<ind_impl<ind>>();
    static_assert(is_derived_node<node_impl<T>, decltype(ref.node)>::value, "can't assign arr ind to non-derived type");
    auto t = new T();
    ref.node.impl = t;
    return *t;
  }
};

template<key_ref_t... refs>
struct obj_impl: obj_t {

  static_assert(is_ref_unique<refs...>::value, "keys for obj are not unique");

  std::tuple<refs...> nodes;
  static constexpr size_t size = std::tuple_size<std::tuple<refs...>>::value;

  // Extension

  template<key_ref_t... srefs>
  using ext = obj_impl<refs..., srefs...>;

  // Indexing Methods for Both!

  template<ctom::key_t ref> struct index {
    static constexpr size_t value = ctom::index_refs<0, ref, refs...>::value;
  };

  template<ctom::key_t ref>
  constexpr auto& get() {
    static_assert(is_ref_contained<ref, refs...>::value, "ref not found");
    return std::get<index<ref>::value>(nodes);
  }

  template<constexpr_string ref>
  constexpr auto& get() {
    return get<key_impl<ref>>().node.impl->value;
  }

  template<ctom::key_t T>
  auto& operator[](T){
    return *(get<T>().node.impl);
  };

  struct for_node {
    template<typename F>
    static constexpr void iter(F&& f){
      for_types<refs...>(f);
    }
  };

  // Instance Value Assignment

  template<constexpr_string key, typename T>
  auto& val(const T& t){
    auto& ref = get<key_impl<key>>();
  //  static_assert(std::is_same<val_impl<T>, decltype(ref.node.impl)>::value, "can't assign val key to improper type");
    auto i = new val_impl<T>();
    ref.node.impl = i;
    ref.node.impl->value = t;
    return ref.node.impl->value;
  }

  template<constexpr_string key, typename T>
  auto& obj(){
    auto& ref = get<key_impl<key>>();
    static_assert(is_derived_node<node_impl<T>, decltype(ref.node)>::value, "can't assign obj key to non-derived type");
    auto t = new T();
    ref.node.impl = t;
    return *t;
  }

  template<constexpr_string key, typename T>
  auto& arr(){
    auto& ref = get<key_impl<key>>();
    static_assert(is_derived_node<node_impl<T>, decltype(ref.node)>::value, "can't assign arr key to non-derived type");
    auto t = new T();
    ref.node.impl = t;
    return *t;
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

template<ctom::ind_key_t ref, val_t T>
struct printer<ref_val<ref, T>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<T::type<<": ";
    std::cout<<ref::val<<"\n";
  }
};

template<ctom::ind_key_t ref, arr_t T>
struct printer<ref_arr<ref, T>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<T::type<<": ";
    std::cout<<ref::val<<"\n";
    T::for_node::iter([shift]<typename N>(){
      printer<N>::print(shift+1);
    });
  }
};

template<ctom::ind_key_t ref, obj_t T>
struct printer<ref_obj<ref, T>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<T::type<<": ";
    std::cout<<ref::val<<"\n";
    T::for_node::iter([shift]<typename N>(){
      printer<N>::print(shift+1);
    });
  }
};

// Print an Object-Type Directly

template<obj_t T>
struct printer<T>{
  static void print(size_t shift = 0){
    T::for_node::iter([shift]<typename N>(){
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

template<ctom::ind_key_t ref, val_t T>
void print(ref_val<ref, T>& ref_val, size_t shift = 0);
template<ctom::ind_key_t ref, arr_t T>
void print(ref_arr<ref, T>& ref_arr, size_t shift = 0);
template<ctom::ind_key_t ind, obj_t T>
void print(ref_obj<ind, T>& ref_obj, size_t shift = 0);

template<ctom::ind_key_t ref, val_t T>
void print(ref_val<ref, T>& ref_val, size_t shift){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<T::type<<": ";
  std::cout<<ref::val<<" = ";
  if(ref_val.node.impl != NULL)
  std::cout<<ref_val.node.impl->value;
  std::cout<<"\n";
}

template<ctom::ind_key_t ref, arr_t T>
void print(ref_arr<ref, T>& ref_arr, size_t shift){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<T::type<<": ";
  std::cout<<ref::val<<" = ["<<"\n";
  if(ref_arr.node.impl != NULL)
  std::apply([&](auto&&... args){
    (ctom::print(args, shift+1), ...);
  }, ref_arr.node.impl->nodes);
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<"]"<<std::endl;
}

template<ctom::ind_key_t ind, obj_t T>
void print(ref_obj<ind, T>& ref_obj, size_t shift){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<T::type<<": ";
  std::cout<<ind::val<<"\n";
  if(ref_obj.node.impl != NULL)
  std::apply([&](auto&&... args){
    (ctom::print(args, shift+1), ...);
  }, ref_obj.node.impl->nodes);
}

// Entry-Point

template<obj_t T>
void print(T& obj){
  std::apply([](auto&&... args){
    (ctom::print(args), ...);
  }, obj.nodes);
}

template<arr_t T>
void print(T& arr){
  std::apply([](auto&&... args){
    (ctom::print(args), ...);
  }, arr.nodes);
}

} // End of Namespace

template<constexpr_string key>
constexpr auto operator""_key(){
  return ctom::key_impl<key>{};
}

#endif
