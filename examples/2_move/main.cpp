#include "../../src/ctom.hpp"
#include <vector>
#include <set>


	template<size_t a>
	struct test {

	};

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct Foo: ctom::obj_impl<
		ctom::val<"foo-int", int>,
		ctom::val<"foo-float", float>,
		ctom::val<"foo-double", double>
	>{};

	struct Foo_Impl: Foo {
		int& a 		= Foo::val<"foo-int">(10);
		float& b 	= Foo::val<"foo-float">(5.0f);
		double& c = Foo::val<"foo-double">(2.5);
	};

	Foo_Impl foo;
	ctom::print(foo);

	foo.a = 5;
	foo.b = 2.5f;
	foo.c = 1.25;
	ctom::print(foo);

	foo["foo-int"_t] = 4;
	foo.get<"foo-float">() = 3;
	foo.get<"foo-double">() = 3;
	ctom::print(foo);

	// Nested Move

	struct Bar: ctom::obj_impl<
		ctom::obj<"foo", Foo>
	>{};

	struct Bar_Impl: Bar {
		Foo_Impl& foo = Bar::obj<"foo", Foo_Impl>();
	};

	Bar_Impl bar;
	bar["foo"_t]["foo-int"_t] = 9;
	bar["foo"_t]["foo-float"_t] = 2;
	bar["foo"_t]["foo-double"_t] = 3;
	ctom::print(bar);
	ctom::print(foo);

	bar.foo.a = 7;
	ctom::print(bar);


	return 0;

}
