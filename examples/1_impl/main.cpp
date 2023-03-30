#include "../../src/ctom.hpp"
#include <vector>
#include <set>

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct Arr: ctom::arr_impl<
		ctom::_val<0, int>,
		ctom::_val<1, int>,
		ctom::_val<2, int>
	>{};

	struct Foo: ctom::obj_impl<
		ctom::val<"foo-int", int>,
		ctom::val<"foo-float", float>,
		ctom::val<"foo-double", double>
	//	ctom::arr<"float-arr", float>
	>{};

	struct Bar: ctom::obj_impl<
		ctom::obj<"bar-foo", Foo>,
		ctom::val<"bar-char", char>,
		ctom::arr<"int-arr", Arr>
	>{};

	struct Root: ctom::obj_impl <
		ctom::val<"int", int>,
		ctom::val<"float", float>,
		ctom::val<"double", double>,
	//	ctom::arr<"foo", Foo>,
		ctom::obj<"bar", Bar>
	>{};

	// Concrete Implementations

	struct Foo_Impl: Foo {
		int a 							= Foo::val<"foo-int">(10);
		float b 						= Foo::val<"foo-float">(5.0f);
		double c 						= Foo::val<"foo-double">(2.5);
	};

	struct Bar_Impl: Bar {
		Foo_Impl foo	= Bar::obj<"bar-foo">(Foo_Impl{});
		char x 				= Bar::val<"bar-char">('y');
	};

	struct Root_Impl: Root {
		Bar_Impl bar = Root::obj<"bar">(Bar_Impl{});
//		std::vector<Foo_Impl> foo = Root::arr<"foo">({Foo_Impl{}});
	};

	// Single Instantiation

	Root_Impl root;
	ctom::print(root);

	return 0;

}
