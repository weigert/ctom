#ifndef CTOM
#define CTOM

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <string>
#include <iostream>

namespace ctom {

/*
================================================================================
                        Node and Node-Type Declarations
================================================================================
These are the base-concepts through which we constrain the object models.
*/

struct node_base{};
struct val_base{ static constexpr char const* type = "val"; };
struct arr_base{ static constexpr char const* type = "arr"; };
struct obj_base{ static constexpr char const* type = "obj"; };

template<typename T> concept node_t = std::derived_from<T, ctom::node_base>;
template<typename T> concept val_t = std::derived_from<T, ctom::val_base>;
template<typename T> concept arr_t = std::derived_from<T, ctom::arr_base>;
template<typename T> concept obj_t = std::derived_from<T, ctom::obj_base>;
template<typename T> concept impl_t = val_t<T>|| arr_t<T> || obj_t<T>;

/*
================================================================================
                          constexpr_string helper
================================================================================
This allows us to use string literals as template parameters.
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
                  Key, Index and Reference Type Implementations
================================================================================
These are types which are used to index tuples, by using refs to tie
key and index to node types.
*/

struct ind_base{};
struct key_base{};
struct ref_base{};

template<typename T> concept ind_t = std::derived_from<T, ctom::ind_base>;
template<typename T> concept key_t = std::derived_from<T, ctom::key_base>;
template<typename T> concept ind_key_t = ctom::ind_t<T> || ctom::key_t<T>;

template<typename T> concept ref_t = std::derived_from<T, ctom::ref_base>;
template<typename T> concept ind_ref_t = ctom::ref_t<T> && T::is_ind;
template<typename T> concept key_ref_t = ctom::ref_t<T> && T::is_key;

template<size_t S>
struct ind_impl: ind_base {
  static constexpr auto val = S;
};

template<constexpr_string S>
struct key_impl: key_base {
  static constexpr auto val = S;
};

template<ind_key_t IK, node_t Node>
struct ref_impl: ref_base {
  static constexpr auto key = IK::val;
  static constexpr auto ind = IK::val;
  static constexpr bool is_ind = ctom::ind_t<IK>;
  static constexpr bool is_key = ctom::key_t<IK>;
  Node node;
};

/*
================================================================================
                Node Implementations and Interpretation Rules
================================================================================
The node implementation is templated by a base-model implementation, storing
a pointer to it so we can also have derived types.

If the type assigned to a node is unknown / not an implementation base-type,
we find out which the intended interpretation type is using an interpret rule.
*/

// Node-Type Forward Declarations

template<typename T> struct val_impl;
template<ind_ref_t... T> struct arr_impl;
template<key_ref_t... T> struct obj_impl;

// Templated Interpretation Rules

template<typename T>
struct rule {
  typedef val_impl<T> type;
};

template<arr_t T>
struct rule<T>{
  typedef T type;
};

template<obj_t T>
struct rule<T> {
  typedef T type;
};

// Node Implementation w. Assignment Operator

template<impl_t T>
struct node_impl: node_base {
  static constexpr auto type = T::type;
  T* impl = NULL;

  template<typename V>
  void operator=(V& v){
    if(impl == NULL)
      impl = new typename rule<V>::type(v);
    *impl = v;
  }
};

/*
================================================================================
                Implementation Forward Declarations and Aliases
================================================================================
Alias structs for easier construction of the complex, specific specialized types.

*/

// Key and Index Aliases

struct ind_alias_base{};
struct key_alias_base{};

template<typename T> concept ind_alias_t = std::derived_from<T, ctom::ind_alias_base>;
template<typename T> concept key_alias_t = std::derived_from<T, ctom::key_alias_base>;

// Key-Based Aliases

template<constexpr_string ref, typename T> 
struct key_alias: key_alias_base {
  typedef key_impl<ref> key_t;
  typedef node_impl<typename rule<T>::type> node_t;
};

// Full Key-Alias Resolution

template<constexpr_string ref, typename T> 
using key = key_alias<ref, T>;

template<key_alias_t... keys> 
using obj = obj_impl<ctom::ref_impl<typename keys::key_t, typename keys::node_t>...>;

// Ind-Based Aliases

template<size_t N, typename T>
struct ind_alias: ind_alias_base {
  typedef ind_impl<N> ind_t;
  typedef node_impl<typename rule<T>::type> node_t;
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

template <size_t N, typename T>
using arr = std::decay_t<decltype(ind_seq<T, N>())>;

/*
================================================================================
                          Compile-Time Helper Values  
================================================================================
These are template-metaprogramming structs which run checks on various types.
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
                            Node-Type Implementations
================================================================================
*/

template<typename T>
struct val_impl: val_base {
  T* value;
  val_impl(T& t) noexcept {
		value = &t;
	}
  void operator=(T& v){
    if(value != NULL)
    *value = v;
  }
  void operator=(T&& v){
    if(value != NULL)
    *value = v;
  }
};

template<ind_ref_t... refs>
struct arr_impl: arr_base {

  static_assert(is_ref_unique<refs...>::value, "references are not unique");

  std::tuple<refs...> nodes;
  static constexpr size_t size = std::tuple_size<decltype(nodes)>::value;

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

  template <size_t N, typename T>
  using ext = std::decay_t<decltype(ind_seq<T, N + size>())>;

  template<ctom::ind_key_t ind> struct index {
    static_assert(ctom::ind_t<ind>, "can't index array with non-index type");
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

  template<size_t _ind>
  auto& val(){
    auto& ref = get<ind_impl<_ind>>();
    return ref.node;
  }

  // Special Constructors

  arr_impl(){}

  template<size_t N, typename T>
  void assign(T& t){
    std::get<N>(nodes).node = t;
  }

  template<size_t N = 0, typename T, typename... Ts>
  void assign(T& t, Ts&... ts){
    std::get<N>(nodes).node = t;
    assign<N+1>(ts...);
  }

  template<typename T>
  arr_impl(T& t){
    assign<0>(t);
  }

  template<typename... Ts>
  arr_impl(Ts&... ts){
    static_assert(sizeof...(Ts) <= size, "too many arguments");
    assign<0>(ts...);
  }

  template<typename T, size_t N>
  arr_impl(T(&t)[N]){
    static_assert(N <= size, "array-size mismatch");
    size_t n = 0;
    for_refs([&t, &n](auto&& ref){
      ref.node = t[n++];
    });
  }
};

template<key_ref_t... refs>
struct obj_impl: obj_base {

  static_assert(is_ref_unique<refs...>::value, "references are not unique");

  std::tuple<refs...> nodes;
  static constexpr size_t size = std::tuple_size<decltype(nodes)>::value;

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

  template<constexpr_string _key>
  auto& val(){
    auto& ref = get<key_impl<_key>>();
    return ref.node;
  }

  // Special Constructors

  obj_impl(){}

  template<size_t N, typename T>
  void assign(T& t){
    std::get<N>(nodes).node = t;
  }

  template<size_t N = 0, typename T, typename... Ts>
  void assign(T& t, Ts&... ts){
    std::get<N>(nodes).node = t;
    assign<N+1>(ts...);
  }

  template<typename T>
  obj_impl(T& t){
    assign<0>(t);
  }

  template<typename... Ts>
  obj_impl(Ts&... ts){
    static_assert(sizeof...(Ts) <= size, "too many arguments");
    assign<0>(ts...);
  }
};

/*
================================================================================
                        Marshal/Unmarshal Base Types
================================================================================
*/

// Exception Handling

struct parse_exception: public std::exception {
    std::string msg;
    explicit parse_exception(std::string _msg):msg{_msg}{};
    const char* what() const noexcept override {
        return msg.c_str();
    }
};

struct exception: public std::exception {
    std::string msg;
    size_t line = 0;
    explicit exception(size_t line, std::string _msg){
        msg = "line ("+std::to_string(line)+"): "+_msg;
    };
    const char* what() const noexcept override {
        return msg.c_str();
    }
};

// Stream-Types

struct ostream_base{};
struct istream_base{};

template<typename T> concept ostream_t = std::derived_from<T, ctom::ostream_base>;
template<typename T> concept istream_t = std::derived_from<T, ctom::istream_base>;

template<ostream_t>
struct ostream {
    explicit ostream(std::ostream& os):os(os){}
    std::ostream& os;
};

template<istream_t>
struct istream {
    explicit istream(std::istream& is):is(is){}
    std::istream& is;
    std::string cur;
    size_t line = 0;
};

/*
================================================================================
                            Compile-Time Marshal
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

} // End of Namespace

#endif
