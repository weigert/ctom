#ifndef CTOM
#define CTOM

#include <iostream>
#include <type_traits>
#include <tuple>
#include <vector>
#include <initializer_list>
#include "key.hpp"

namespace ctom {

/*
================================================================================
                          CTOM Base Type Definitions
================================================================================
*/

// Base-Types, Base-Type Concepts

struct ref_base{};
struct ind_base{};
struct val_base{};
struct arr_base{};
struct obj_base{};
struct node_base{};

template<typename T> concept ref_type = std::derived_from<T, ctom::ref_base>;
template<typename T> concept ind_type = std::derived_from<T, ctom::ind_base>;
template<typename T> concept val_type = std::derived_from<T, ctom::val_base>;
template<typename T> concept arr_type = std::derived_from<T, ctom::arr_base>;
template<typename T> concept obj_type = std::derived_from<T, ctom::obj_base>;

// References to Base-Type Implementations
//  Acts as an identifier for a ref to
//  specific type implementations

template<constexpr_string ref, ctom::val_type T> struct ref_val: ctom::ref_base {};
template<constexpr_string ref, ctom::arr_type T> struct ref_arr: ctom::ref_base {};
template<constexpr_string ref, ctom::obj_type T> struct ref_obj: ctom::ref_base {};

template<size_t ind, ctom::val_type T> struct ind_val: ctom::ind_base {};
template<size_t ind, ctom::arr_type T> struct ind_arr: ctom::ind_base {};
template<size_t ind, ctom::obj_type T> struct ind_obj: ctom::ind_base {};

// Node-Types and Aliases
//  Nodes are templated by keys referencing their type

// Reference Nodes

template<ctom::ref_type ref> struct node_ref: ctom::node_base {};

template<constexpr_string ref, ctom::val_type T>
struct node_ref<ctom::ref_val<ref, T>> {
  static constexpr char const* type = "val";
  static constexpr char const* key = ref;
  T val;
};

template<constexpr_string ref, ctom::val_type T>
using node_ref_val = ctom::node_ref<ctom::ref_val<ref, T>>;

template<constexpr_string ref, ctom::arr_type T>
struct node_ref<ctom::ref_arr<ref, T>> {
  static constexpr char const* type = "arr";
  static constexpr char const* key = ref;
  T arr;
};

template<constexpr_string ref, ctom::arr_type T>
using node_ref_arr = ctom::node_ref<ctom::ref_arr<ref, T>>;

template<constexpr_string ref, ctom::obj_type T>
struct node_ref<ctom::ref_obj<ref, T>> {
  static constexpr char const* type = "obj";
  static constexpr char const* key = ref;
  T obj;
};

template<constexpr_string ref, ctom::obj_type T>
using node_ref_obj = ctom::node_ref<ctom::ref_obj<ref, T>>;

// Index Nodes

template<ctom::ind_type ind> struct node_ind: ctom::node_base {};

template<size_t ind, ctom::val_type T>
struct node_ind<ctom::ind_val<ind, T>> {
  static constexpr char const* type = "val";
  static constexpr int const index = ind;
  T val;
};

template<size_t ind, ctom::val_type T>
using node_ind_val = ctom::node_ind<ctom::ind_val<ind, T>>;

template<size_t ind, ctom::arr_type T>
struct node_ind<ctom::ind_arr<ind, T>> {
  static constexpr char const* type = "arr";
  static constexpr int const index = ind;
  T arr;
};

template<size_t ind, ctom::arr_type T>
using node_ind_arr = ctom::node_ind<ctom::ind_arr<ind, T>>;

template<size_t ind, ctom::obj_type T>
struct node_ind<ctom::ind_obj<ind, T>> {
  static constexpr char const* type = "obj";
  static constexpr int const index = ind;
  T obj;
};

template<size_t ind, ctom::obj_type T>
using node_ind_obj = ctom::node_ind<ctom::ind_obj<ind, T>>;

// Base Type Implementation Declarations

template<typename T> struct val_impl;     // Val Parameterized by Single Type
template<typename T> struct arr_impl;     // Arr Parameterized by Single Type
template<typename... T> struct obj_impl;  // Obj Parameterized by Multiple Types

template<constexpr_string ref, typename T>
using val = ctom::ref_val<ref, val_impl<T>>;

template<constexpr_string ref, typename T>
using arr = ctom::ref_arr<ref, arr_impl<T>>;

template<constexpr_string ref, typename T>
using obj = ctom::ref_obj<ref, T>;

/*
================================================================================
                              Helper Concepts
================================================================================
*/

// Is Derived Ref
//  Checks if two reference-types have the same key and point to
//  a derived type A and a base type B

template<typename A, typename B>
struct is_derived {
  static constexpr const bool value = std::is_base_of<B, A>::value || std::is_same<A, B>::value;
};

template<ref_type A, ref_type B>
struct is_derived_ref;

template<
  template<constexpr_string, typename> typename refA, constexpr_string keyA, typename A,
  template<constexpr_string, typename> typename refB, constexpr_string keyB, typename B
>
struct is_derived_ref<refA<keyA, A>, refB<keyB, B>> {
  static constexpr const bool value = is_derived<A, B>::value
    && is_same_key<keyA, keyB>::value;
};

template<
  template<constexpr_string, typename> typename refA, constexpr_string keyA, typename A,
  template<constexpr_string, typename> typename refB, constexpr_string keyB, typename B
>
struct is_derived_ref<refA<keyA, arr_impl<A>>, refB<keyB, arr_impl<B>>> {
  static constexpr const bool value = is_derived<A, B>::value
    && is_same_key<keyA, keyB>::value;
};

// Ref in Argument Set

template <typename T, typename... List>
struct is_contained;

template <typename T>
struct is_contained<T> {
  static constexpr bool value = false;
};

template <typename T, typename Head, typename... Tail>
struct is_contained<T, Head, Tail...>{
  static constexpr bool value =
  std::is_same<T, Head>::value || is_derived<T, Head>::value || is_contained<T, Tail...>::value;
};

// Is Derived Ref Contained

template <ref_type T, ref_type... List>
struct is_derived_ref_contained;

template <ref_type T>
struct is_derived_ref_contained<T> {
  static constexpr bool value = false;
};

template <
  template<constexpr_string, typename> typename refA, constexpr_string keyA, typename A,
  template<constexpr_string, typename> typename refB, constexpr_string keyB, typename B,
  ref_type... Tail
> struct is_derived_ref_contained<refA<keyA, A>, refB<keyB, B>, Tail...>{
  static constexpr bool value = is_derived_ref<refA<keyA, A>, refB<keyB, B>>::value
    || is_derived_ref_contained<refA<keyA, A>, Tail...>::value;
};

// Is Ref Key Contained

template <ref_type T, ref_type... List>
struct is_ref_key_contained;

template <ref_type T>
struct is_ref_key_contained<T> {
  static constexpr bool value = false;
};

template <
  template<constexpr_string, typename> typename refA, constexpr_string keyA, typename A,
  template<constexpr_string, typename> typename refB, constexpr_string keyB, typename B,
  ref_type... Tail
> struct is_ref_key_contained<refA<keyA, A>, refB<keyB, B>, Tail...>{
  static constexpr bool value = is_same_key<keyA, keyB>::value
    || is_ref_key_contained<refA<keyA, A>, Tail...>::value;
};

// Is Key Unique

template <ref_type... refs>
struct is_ref_key_unique;

template <>
struct is_ref_key_unique<> {
    static constexpr bool value = true;
};

template <typename ref, typename... refs>
struct is_ref_key_unique<ref, refs...>{
  static constexpr bool value = !is_ref_key_contained<ref, refs...>::value && is_ref_key_unique<refs...>::value;
};

// Tuple-of-Ref Indexer, which will index based on derived type

template <ref_type T, size_t N, ref_type... Args>
struct ref_index;

template <ref_type T, size_t N, ref_type... Args>
struct ref_index {
    static constexpr size_t value = N;
};

template <ref_type T, size_t N>
struct ref_index<T, N>{
    static constexpr size_t value = N;
};

template <ref_type T, size_t N, ref_type U, ref_type... Args>
requires(!is_derived_ref<T, U>::value)
struct ref_index<T, N, U, Args...> {
    static constexpr size_t value = ref_index<T, N + 1, Args...>::value;
};

template <ref_type T, size_t N, ref_type U, ref_type... Args>
requires(is_derived_ref<T, U>::value)
struct ref_index<T, N, U, Args...> {
    static constexpr size_t value = N;
};

// Type-List Iterator

template <typename... Ts, typename F>
constexpr void for_types(F&& f){
    (f.template operator()<Ts>(), ...);
}

// Ref-Type Pack

template <typename... Ts> struct is_refs;

template <typename T> struct is_refs<T> {
  static constexpr bool value = false;
};

template <ref_type T> struct is_refs<T> {
  static constexpr bool value = true;
};

template <typename T, typename... Ts> struct is_refs<T, Ts...> {
  static constexpr bool value = is_refs<T>::value && is_refs<Ts...>::value;
};

/*
================================================================================
                              Implementations
================================================================================
*/

// Value

// Implementable Types

template<typename V>
concept is_val =
    std::is_same_v<V, bool>
||  std::is_same_v<V, char>
||  std::is_same_v<V, int>
||  std::is_same_v<V, long>
||  std::is_same_v<V, size_t>
||  std::is_same_v<V, float>
||  std::is_same_v<V, double>;

template<typename V>
struct val_impl: val_base {
  static_assert(is_val<V>, "template type for val_impl is not scalar");
  V value;
};

// Array

template<typename V>
concept is_arr = is_val<V> || obj_type<V>;

template<typename T>
struct arr_impl: arr_base {
   std::vector<T> values;

   struct nodes {
     std::tuple<node_ind<ind_val<0, val_impl<T>>>> nodes;
   };
   //

};

// Object

/*
  I can assign a derived type to the obj,
  because it is ONLY DEFINED BY THE TEMPLATE
  i.e. the type is a valid type of that template.

  I can either use fixed-size arrays, or I can
  use some kind of templating index magic?

  I should refactor the implementation,
  so that I have an implementation of a reference containing
  object, and an actual implementation which contains the reference containing
  object.
  This way everything would fit together nicely.
  I could have an index-based container and a key-based container
  (object, array) and combine them as I wish.
*/

template<typename... refs>
struct obj_impl: obj_base {

  static_assert(is_refs<refs...>::value, "template parameters for obj are not of type ref");
  static_assert(is_ref_key_unique<refs...>::value, "ref parameter keys for obj are not unique");

  std::tuple<node_ref<refs>...> nodes;

  static constexpr size_t size = std::tuple_size<std::tuple<node_ref<refs>...>>::value;

  template<ref_type ref> struct index {
    static constexpr size_t value = ref_index<ref, 0, refs...>::value;
  };

  template<ref_type ref>
  auto& get() {
    static_assert(index<ref>::value < size, "index is out of bounds");
    return std::get<index<ref>::value>(nodes);
  }

  struct for_node {
    template<typename F>
    static constexpr void iter(F&& f){
      for_types<node_ref<refs>...>(f);
    }
  };

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

  template<ref_type... srefs>
  using ext = obj_impl<refs..., srefs...>;

};

/*
================================================================================
                          Marshalling / Unmarshalling
================================================================================
*/

// Fully Static Printing

template<typename T>
struct printer {
  static void print(size_t shift = 0);
};

template<constexpr_string ref, val_type T>
struct printer<node_ref_val<ref, T>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<node_ref_val<ref, T>::type<<": ";
    std::cout<<ref<<"\n";
  }
};

template<constexpr_string ref, arr_type T>
struct printer<node_ref_arr<ref, T>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<node_ref_arr<ref, T>::type<<": ";
    std::cout<<ref<<"\n";
  }
};

template<constexpr_string ref, obj_type T>
struct printer<node_ref_obj<ref, T>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<node_ref_obj<ref, T>::type<<": ";
    std::cout<<ref<<"\n";
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

template<constexpr_string ref, val_type T>
void print(node_ref_val<ref, T>& node, size_t shift = 0){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<node_ref_val<ref, T>::type<<": ";
  std::cout<<ref<<" = ";
  std::cout<<node.val.value<<"\n";
}

template<constexpr_string ref, arr_type T>
void print(node_ref_arr<ref, T>& node, size_t shift = 0){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<node_ref_arr<ref, T>::type<<": ";
  std::cout<<ref<<" = ["<<"\n";
  for(size_t n = 0; n < node.arr.values.size(); n++){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<"  "<<node.arr.values[n]<<",\n";
  }
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<"]"<<std::endl;
}

template<constexpr_string ref, obj_type T>
void print(node_ref_obj<ref, T>& node, size_t shift = 0){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<node_ref_obj<ref, T>::type<<": ";
  std::cout<<ref<<"\n";

  std::apply([&](auto&&... args){
    (ctom::print(args, shift+1), ...);
  }, node.obj.nodes);
}

template<obj_type T>
void print(T& obj){
  std::apply([](auto&&... args){
    (ctom::print(args), ...);
  }, obj.nodes);
}

}

#endif
