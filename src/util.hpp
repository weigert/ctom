
// Variadic Is Same
template <typename C, typename... Ts> struct is_same;

template <typename T> struct is_same<T> {
  static constexpr bool value = false;
};

template <typename C, typename T> struct is_same<C, T> {
  static constexpr bool value = std::is_same<C, T>::value;
};

template <typename C, typename T, typename... Ts>  struct is_same<C, T, Ts...>{
  static constexpr bool value = is_same<C, T>::value && is_same<C, Ts...>::value;
};

template <typename... Ts, typename F>
constexpr void for_types(F&& f){
    (f.template operator()<Ts>(), ...);
}
