#include "../../src/ctom.hpp"
#include <vector>
#include <set>

int main( int argc, char* args[] ) {

	// Combined Definition / Implementation

	struct Foo: ctom::obj<
		ctom::key<"foo-int", int>,
		ctom::key<"foo-float", float>,
		ctom::key<"foo-double", double>
	>{
		Foo(){
			this->val<"foo-int">() = a;
			this->val<"foo-float">() = b;
			this->val<"foo-double">() = c;
		}

		int a = 0;
		float b = 0;
		double c = 0;
	};

	// Compound-Assignment: Arrays

	using Bar_t = ctom::arr<4, int>;

	struct Bar: Bar_t {
		Bar():Bar_t(arr){};
		int arr[4] = {0, 1, 2, 3};
	};

	// Compound-Assignment: Structs

	using Baz_t = ctom::obj<
		ctom::key<"bar-foo", Foo>,
		ctom::key<"bar-char", char>,
		ctom::key<"bar-barr", Bar>
	>;

	struct Baz: Baz_t {
		Baz():Baz_t(foo, c, bar){};
		Foo foo;
		char c = 'x';
		Bar bar;
	};

	return 0;

}
