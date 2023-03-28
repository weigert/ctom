#include <iostream>


#include <type_traits>
#include <iomanip>
#include <array>

namespace ctom {

/*
ctom.hpp
compile-time object model
author: Nicholas McDonald 2023

this namespace lets you define a compile-time object model using template
meta-programming techniques. The object-model allows for fast (un)marshalling
of various data formats, and is similar to semantic tags in golang.

the original intent was a type-safe compile-time yaml parser.
*/

// Basic Ref-Types

struct ref_base{};
struct val_base{
  static constexpr const char * type = "val";
};
struct arr_base{
  static constexpr const char * type = "arr";
};
struct obj_base{
  static constexpr const char * type = "obj";
};

// Base-Type Concepts
//  Lets us defined templated derived types
//  and still easily restrict parameters

template<typename T>
concept ref_type = std::derived_from<T, ref_base>;
template<typename T>
concept val_type = std::derived_from<T, val_base>;
template<typename T>
concept arr_type = std::derived_from<T, arr_base>;
template<typename T>
concept obj_type = std::derived_from<T, obj_base>;

// Constant Expression String
//  Lets us use a string-literal as a template parameter

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
template<unsigned N> constexpr_string(char const (&)[N]) ->constexpr_string<N-1>;

template<constexpr_string S>
struct constexpr_key{};

template<constexpr_string A, constexpr_string B>
struct is_same_key {
  static constexpr const bool value = std::is_same<constexpr_key<A>, constexpr_key<B>>::value;
};

// References to Base-Type Implementations
//  Acts as an identifier for a ref to
//  specific type implementations

template<constexpr_string ref, val_type V>
struct ref_val_impl: ref_base {};

template<constexpr_string Key, arr_type T>
struct ref_arr_impl: ref_base {};

template<constexpr_string Key, obj_type T>
struct ref_obj_impl: ref_base {};

// Is Derived Ref
//  Checks if two reference-types have the same key and point to
//  a derived type A and a base type B

template<typename A, typename B>
struct is_derived {
  static constexpr const bool value = std::is_base_of<B, A>::value;
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
  std::is_same<T, Head>::value || std::is_base_of<Head, T>::value || is_contained<T, Tail...>::value;
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

/*
================================================================================
                        Template Meta Object Model
================================================================================
*/

// Node-Type Declarations
//  Structs which are templated by a specific ref type
//  give us specific node types (with a required ref)

template<ref_type ref> struct node {};

template<constexpr_string ref, val_type T>
struct node<ref_val_impl<ref, T>> {
  static constexpr const char* type = "val";
  static constexpr char const* key = ref;
  T val;
};

template<constexpr_string ref, arr_type T>
struct node<ref_arr_impl<ref, T>> {
  static constexpr const char* type = "arr";
  static constexpr char const* key = ref;
  T arr;
};

template<constexpr_string ref, obj_type T>
struct node<ref_obj_impl<ref, T>> {
  static constexpr const char* type = "obj";
  static constexpr char const* key = ref;
  T obj;
};

// Node Type Aliases

template<constexpr_string ref, val_type T>
using node_val = node<ref_val_impl<ref, T>>;

template<constexpr_string ref, arr_type T>
using node_arr = node<ref_arr_impl<ref, T>>;

template<constexpr_string ref, obj_type T>
using node_obj = node<ref_obj_impl<ref, T>>;

/*
================================================================================
                              Implementations
================================================================================
*/

template<typename V>
concept is_value =
    std::is_same_v<V, bool>
||  std::is_same_v<V, char>
||  std::is_same_v<V, int>
||  std::is_same_v<V, long>
||  std::is_same_v<V, size_t>
||  std::is_same_v<V, float>
||  std::is_same_v<V, double>;

template<is_value V>
struct val_impl: val_base {
  V value;
};

template<constexpr_string ref, is_value V>
using ref_val = ref_val_impl<ref, val_impl<V>>;
template<constexpr_string ref, is_value V>
using val = ref_val<ref, V>;

// Array

template<typename T>
struct arr_impl: arr_base {
  // std::vector<T> values;
};

template<constexpr_string ref, typename T>
using ref_arr = ref_arr_impl<ref, arr_impl<T>>;
template<constexpr_string ref, typename T>
using arr = ref_arr<ref, arr_impl<T>>;

// Object

template<constexpr_string Key, obj_type T>
using ref_obj = ref_obj_impl<Key, T>;
template<constexpr_string Key, obj_type T>
using obj = ref_obj<Key, T>;

template<ref_type... refs>
requires(is_ref_key_unique<refs...>::value)
struct obj_impl: obj_base {

  std::tuple<node<refs>...> nodes;

  static constexpr size_t size = std::tuple_size<std::tuple<node<refs>...>>::value;

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
      for_types<node<refs>...>(f);
    }
  };

  // Instance Value Assignment

  template<constexpr_string ref, typename V>
  V val(const V& v){
    static_assert(is_value<V>, "type is not a value type");
    static_assert(is_contained<ref_val<ref, V>, refs...>::value, "key for yaml::val does not exist in yaml::obj");
    auto& node = get<ref_val<ref, V>>();
    node.val.value = v;
    return std::move(v);
  }

  template<constexpr_string ref, typename T>
  T obj(const T& v){
    static_assert(obj_type<T>, "type is not a derived type of yaml::obj");
    static_assert(is_derived_ref_contained<ref_obj<ref, T>, refs...>::value, "key for yaml::obj does not exist in yaml::obj");
    auto& node = get<ref_obj<ref, T>>();
    node.obj = v;
    return std::move(v);
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
struct printer<node<ref_val_impl<ref, T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<val_base::type<<": ";
    std::cout<<ref<<"\n";
  }
};

template<constexpr_string ref, arr_type T>
struct printer<node<ref_arr_impl<ref, T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<arr_base::type<<": ";
    std::cout<<ref<<"\n";
  }
};

template<constexpr_string ref, obj_type T>
struct printer<node<ref_obj_impl<ref, T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<obj_base::type<<": ";
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
void print(node_val<ref, T>& node, size_t shift = 0){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<val_base::type<<": ";
  std::cout<<ref<<" = ";
  std::cout<<node.val.value<<"\n";
}

template<constexpr_string ref, arr_type T>
void print(node_arr<ref, T>& node, size_t shift = 0){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<arr_base::type<<": ";
  std::cout<<ref<<"\n";
}

template<constexpr_string ref, obj_type T>
void print(node_obj<ref, T>& node, size_t shift = 0){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<obj_base::type<<": ";
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
