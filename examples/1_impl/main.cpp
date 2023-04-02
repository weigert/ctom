#include "../../src/ctom.hpp"
#include <vector>
#include <set>

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct Arr: ctom::arr<
		ctom::ind::val<0, int>,
		ctom::ind::val<1, int>,
		ctom::ind::val<2, int>
	>{};

	struct Foo: ctom::obj<
		ctom::key::val<"foo-int", int>,
		ctom::key::val<"foo-float", float>,
		ctom::key::val<"foo-double", double>
	>{};

	struct Bar: ctom::obj<
		ctom::key::obj<"bar-foo", Foo>,
		ctom::key::val<"bar-char", char>,
		ctom::key::arr<"int-arr", Arr>
	>{};

	struct Root: ctom::obj <
		ctom::key::val<"int", int>,
		ctom::key::val<"float", float>,
		ctom::key::val<"double", double>,
		ctom::key::obj<"bar", Bar>
	>{};

	// Concrete Implementations

	struct Foo_Impl: Foo {
		int& a 	 	= Foo::val<"foo-int">(10);
		float& b  = Foo::val<"foo-float">(5.0f);
		double& c = Foo::val<"foo-double">(2.5);
	};

	struct Arr_Impl: Arr {
		int& a = Arr::val<0, int>(9);
		int& b = Arr::val<1, int>(7);
		int& c = Arr::val<2, int>(5);
	};

	struct Bar_Impl: Bar {
		Foo_Impl& foo	= Bar::obj<"bar-foo", Foo_Impl>();
		char& x 			= Bar::val<"bar-char">('y');
		Arr_Impl& arr = Bar::arr<"int-arr", Arr_Impl>();
	};

	struct Root_Impl: Root {
		Bar_Impl& bar = Root::obj<"bar", Bar_Impl>();
	};

	// Single Instantiation

	Root_Impl root;
	root["bar"_key]["bar-foo"_key]["foo-int"_key] = 7;
	ctom::print(root);

	Arr_Impl arr;
	arr[ctom::ind_impl<0>{}] = 6;
	arr[ctom::ind_impl<1>{}] = 3;
	arr[ctom::ind_impl<2>{}] = 0;
	ctom::print(arr);

	Bar_Impl bar;
	bar.x = 'x';
	ctom::print(bar);

	return 0;

}
