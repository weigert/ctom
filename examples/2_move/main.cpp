#include "../../src/ctom.hpp"
#include <vector>
#include <set>

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct Foo: ctom::obj<
		ctom::key::val<"foo-int", int>,
		ctom::key::val<"foo-float", float>,
		ctom::key::val<"foo-double", double>
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

	foo["foo-int"_key] = 4;
	foo.get<"foo-float">() = 3;
	foo.get<"foo-double">() = 3;
	ctom::print(foo);

	// Nested Move

	struct Bar: ctom::obj<
		ctom::key::obj<"foo", Foo>
	>{};

	struct Bar_Impl: Bar {
		Foo_Impl& foo = Bar::obj<"foo", Foo_Impl>();
	};

	Bar_Impl bar;
	bar["foo"_key]["foo-int"_key] = 9;
	bar["foo"_key]["foo-float"_key] = 2;
	bar["foo"_key]["foo-double"_key] = 3;
	ctom::print(bar);
	ctom::print(foo);

	bar.foo.a = 7;
	bar.foo.b = 4.5;
	ctom::print(bar);
	ctom::print(bar.foo);

	Bar_Impl bar2;
	ctom::print(bar2);

	return 0;

}
