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
