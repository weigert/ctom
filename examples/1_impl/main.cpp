#include "../../src/ctom.hpp"
#include <vector>
#include <set>

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct Foo: ctom::obj_impl<
		ctom::val<"foo-int", int>,
		ctom::val<"foo-float", float>,
		ctom::val<"foo-double", double>,
		ctom::arr<"int-arr", int>,
		ctom::arr<"float-arr", float>
	>{};

	struct Bar: ctom::obj_impl<
		ctom::obj<"bar-foo", Foo>,
		ctom::val<"bar-char", char>
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
		float b 						= Foo::val<"foo-float">(10.0f);
		double c 						= Foo::val<"foo-double">(10.0);
		std::vector<int> f 	= Foo::arr<"int-arr">({1, 2, 3});
		std::set<float> s 	= Foo::arr<"float-arr">({0.1f, 0.2f, 0.3f});
		int x, y;						// additional parameters don't matter
	};

	struct Bar_Impl: Bar {
		// derived type member can be mapped
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
