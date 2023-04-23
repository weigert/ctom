#ifndef CTOM
#define CTOM

#include <iostream>
#include <type_traits>
#include <tuple>
#include <string>

namespace ctom {

/*
================================================================================
                        Node-Types and Implementations
================================================================================
*/

struct node_base{};
struct val_base{ static constexpr char const* type = "val"; };
struct arr_base{ static constexpr char const* type = "arr"; };
struct obj_base{ static constexpr char const* type = "obj"; };

template<typename T> concept node_t = std::derived_from<T, ctom::node_base>;
template<typename T> concept val_t = std::derived_from<T, ctom::val_base>;
template<typename T> concept arr_t = std::derived_from<T, ctom::arr_base>;
template<typename T> concept obj_t = std::derived_from<T, ctom::obj_base>;

template<typename T>
struct node_impl: node_base {
  static constexpr auto type = T::type;

  T* impl = NULL;
  ~node_impl(){
    if(impl != NULL) delete impl;
  }
  template<typename V>
  void operator=(const V& v){
    *impl = v;
  }
  template<typename V>
  void operator=(const V&& v){
    *impl = v;
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
                          constexpr_string helper
================================================================================
*/

template<size_t N> struct constexpr_string {

  char value[N + 1] = {};
  constexpr constexpr_string(const char (&_value)[N+1]) {
      for (size_t n = 0; n < N; ++n)
        value[n] = _value[n];
  }

  constexpr operator char const*() const { return value; }
  constexpr char operator[](size_t n) const noexcept {
		return value[n];
	}
  constexpr size_t size() const noexcept {
		return N;
	}
  constexpr const char * begin() const noexcept {
    return value;
  }
  constexpr const char * end() const noexcept {
    return value + size();
  }

};
template<size_t N> constexpr_string(char const (&)[N]) -> constexpr_string<N-1>;

/*
================================================================================
                      Key, Index and Reference Types
================================================================================
*/

struct ind_base{};
struct key_base{};
struct ref_base{};

template<typename T> concept ind_t = std::derived_from<T, ctom::ind_base>;
template<typename T> concept key_t = std::derived_from<T, ctom::key_base>;
template<typename T> concept ref_t = std::derived_from<T, ctom::ref_base>;
template<typename T> concept ind_key_t = ind_t<T> || key_t<T>;

template<ind_key_t T, node_t N>
struct ref_impl: ref_base {
  static constexpr auto key = T::val;
  static constexpr auto ind = T::val;
  N node;
};

template<size_t S>
struct ind_impl: ind_base {
  static constexpr auto val = S;
};

template<constexpr_string S>
struct key_impl: key_base {
  static constexpr auto val = S;
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

/*
================================================================================
                Implementation Forward Declarations and Aliases
================================================================================
*/

// Node-Implementation Forward Declarations

template<typename T> struct val_impl;
template<ind_ref_t... T> struct arr_impl;
template<key_ref_t... T> struct obj_impl;

// Key and Index Aliases

struct ind_alias_base{};
struct key_alias_base{};

template<typename T> concept ind_alias_t = std::derived_from<T, ctom::ind_alias_base>;
template<typename T> concept key_alias_t = std::derived_from<T, ctom::key_alias_base>;

// Key-Based Aliases

template<constexpr_string ref, typename T> 
struct key_alias;

template<constexpr_string ref, typename T> 
struct key_alias: key_alias_base {
  typedef key_impl<ref> key_t;
  typedef node_impl<val_impl<T>> node_t;
};

template<constexpr_string ref, arr_t T> 
struct key_alias<ref, T>: key_alias_base {
  typedef key_impl<ref> key_t;
  typedef node_impl<T> node_t;
};

template<constexpr_string ref, obj_t T> 
struct key_alias<ref, T>: key_alias_base {
  typedef key_impl<ref> key_t;
  typedef node_impl<T> node_t;
};

// Full Key-Alias Resolution

template<constexpr_string ref, typename T> 
using key = key_alias<ref, T>;

template<key_alias_t... keys> 
using obj = obj_impl<ctom::ref_impl<typename keys::key_t, typename keys::node_t>...>;

// Ind-Based Aliases

template<size_t N, typename T>
struct ind_alias;

template<size_t N, typename T>
struct ind_alias: ind_alias_base {
  typedef ind_impl<N> ind_t;
  typedef node_impl<val_impl<T>> node_t;
};

template<size_t N, arr_t T>
struct ind_alias<N, T>: ind_alias_base {
  typedef ind_impl<N> ind_t;
  typedef node_impl<T> node_t;
};

template<size_t N, obj_t T>
struct ind_alias<N, T>: ind_alias_base {
  typedef ind_impl<N> ind_t;
  typedef node_impl<T> node_t;
};

// Full Ind-Alias Resolution

template<size_t N, typename T> 
using ind = ind_alias<N, T>;

template<ind_alias_t... inds>
using arr_ind_set = arr_impl<ctom::ref_impl<typename inds::ind_t, typename inds::node_t>...>;

// Ind-Sequence Based Aliases

template <typename T, size_t N, size_t... Is>
auto ind_seq() {
    if constexpr (N == 0) return arr_ind_set<ind<Is, T>...>(); // end case
    else return ind_seq<T, N-1, N-1, Is...>(); // recursion
}

template <typename T, size_t N>
using arr = std::decay_t<decltype(ind_seq<T, N>())>;

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
                          Node Member Implementations
================================================================================
*/

// Value

template<typename T>
struct val_impl: val_base {
  T value;
  val_impl(){}
  val_impl(T&& t) noexcept {
		value = t;
	}
  void operator=(const T& v){
    value = v;
  }
  void operator=(const T&& v){
    value = v;
  }
};

// Array

template<ind_ref_t... refs>
struct arr_impl: arr_base {

  static_assert(is_ref_unique<refs...>::value, "indices for arr are not unique");

  std::tuple<refs...> nodes;
  static constexpr size_t size = std::tuple_size<std::tuple<refs...>>::value;

  template <typename T, size_t N>
  using ext = std::decay_t<decltype(ind_seq<T, N + size>())>;

  template<ctom::ind_t ind> struct index {
    static constexpr size_t value = ctom::index_refs<0, ind, refs...>::value;
  };

  template<ctom::ind_t ind>
  constexpr auto& get() {
    static_assert(is_ref_contained<ind, refs...>::value, "index is out of bounds");
    return std::get<index<ind>::value>(nodes);
  }

  template<size_t ind>
  constexpr auto& get() {
    return *get<ind_impl<ind>>().node.impl;
  }

  // Static and Non-Static Iteration

  struct for_type {
    template<typename F>
    static constexpr void iter(F&& f){
      (f.template operator()<refs>(), ...);
    }
  };

  template<typename F>
  void for_refs(F&& f){
    std::apply([&](auto&&... ref){
      (f.template operator()(ref), ...);
    }, nodes);
  }

  // Instance Assignment

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

// Object

template<key_ref_t... refs>
struct obj_impl: obj_base {

  static_assert(is_ref_unique<refs...>::value, "keys for obj are not unique");

  std::tuple<refs...> nodes;
  static constexpr size_t size = std::tuple_size<std::tuple<refs...>>::value;

  // Extension

  template<key_alias_t... srefs>
  using ext = obj_impl<refs..., ctom::ref_impl<typename srefs::key_t, typename srefs::node_t>...>;

  // Indexing Methods for Both!

  template<ctom::key_t ref> struct index {
    static constexpr size_t value = ctom::index_refs<0, ref, refs...>::value;
  };

  template<ctom::key_t ref>
  constexpr auto& get() {
    static_assert(is_ref_contained<ref, refs...>::value, "ref not found");
    return std::get<index<ref>::value>(nodes);
  }

  template<constexpr_string s>
  constexpr auto& get() {
    return *get<key_impl<s>>().node.impl;
  }

  // Static and Non-Static Iteration

  struct for_type {
    template<typename F>
    static constexpr void iter(F&& f){
      (f.template operator()<refs>(), ...);
    }
  };

  template<typename F>
  void for_refs(F&& f){
    std::apply([&](auto&&... ref){
      (f.template operator()(ref), ...);
    }, nodes);
  }

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

template<ctom::ind_key_t IK, val_t T>
struct printer<ref_impl<IK, node_impl<T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<T::type<<": ";
    std::cout<<"["<<IK::val<<"]\n";
  }
};

template<ctom::ind_key_t IK, arr_t T>
struct printer<ref_impl<IK, node_impl<T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<T::type<<": ";
    std::cout<<"["<<IK::val<<"]\n";
    T::for_type::iter([shift]<typename N>(){
      printer<N>::print(shift+1);
    });
  }
};

template<ctom::ind_key_t IK, obj_t T>
struct printer<ref_impl<IK, node_impl<T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<T::type<<": ";
    std::cout<<"["<<IK::val<<"]\n";
    T::for_type::iter([shift]<typename N>(){
      printer<N>::print(shift+1);
    });
  }
};

// Print an Object-Type Directly

template<obj_t T>
struct printer<T>{
  static void print(size_t shift = 0){
    T::for_type::iter([shift]<typename N>(){
      printer<N>::print(shift);
    });
  }
};

template<arr_t T>
struct printer<T>{
  static void print(size_t shift = 0){
    T::for_type::iter([shift]<typename N>(){
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

template<val_t T>
void print(T&, std::string prefix = "");
template<arr_t T>
void print(T&, std::string prefix = "");
template<obj_t T>
void print(T&, std::string prefix = "");

template<val_t T>
void print(T& val, std::string prefix){
  std::cout<<val.value;
}

template<arr_t T>
void print(T& arr, std::string prefix){

  arr.for_refs([prefix](auto&& ref){

    if(ref.node.type == "val"){
      std::cout<<prefix;
      std::cout<<ref.node.type<<": ";
      std::cout<<"["<<ref.ind<<"] = ";
      if(ref.node.impl != NULL)
        ctom::print(*ref.node.impl, prefix+"  ");
      std::cout<<"\n";
    }

    if(ref.node.type == "arr"){
      std::cout<<prefix;
      std::cout<<ref.node.type<<": ";
      std::cout<<"["<<ref.ind<<"] = ";
      std::cout<<"[\n";
      if(ref.node.impl != NULL)
        ctom::print(*ref.node.impl, prefix+"  ");
      std::cout<<prefix;
      std::cout<<"]\n";
    } 

    if(ref.node.type == "obj"){
      std::cout<<prefix;
      std::cout<<ref.node.type<<": ";
      std::cout<<"["<<ref.ind<<"] = ";
      std::cout<<"\n";
      if(ref.node.impl != NULL)
        ctom::print(*ref.node.impl, prefix+"  ");
    }

  });

}

template<obj_t T>
void print(T& obj, std::string prefix){

  // Iterate over Object References
  
  obj.for_refs([prefix](auto&& ref){
  
    if(ref.node.type == "val"){
      std::cout<<prefix;
      std::cout<<ref.node.type<<": ";
      std::cout<<"\""<<ref.key<<"\" = ";
      if(ref.node.impl != NULL)
        ctom::print(*ref.node.impl, prefix+"  ");
      std::cout<<"\n";
    }
  
    if(ref.node.type == "arr"){
      std::cout<<prefix;
      std::cout<<ref.node.type<<": ";
      std::cout<<"\""<<ref.key<<"\" = ";
      std::cout<<"[\n";
      if(ref.node.impl != NULL)
        ctom::print(*ref.node.impl, prefix+"  ");
      std::cout<<prefix;
      std::cout<<"]\n";
    } 

    if(ref.node.type == "obj"){
      std::cout<<prefix;
      std::cout<<ref.node.type<<": ";
      std::cout<<"\""<<ref.key<<"\" = ";
      std::cout<<"\n";
      if(ref.node.impl != NULL)
        ctom::print(*ref.node.impl, prefix+"  ");
    }

  });

}

} // End of Namespace

#endif
