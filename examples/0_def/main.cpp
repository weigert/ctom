#include "../../src/ctom.hpp"

int main( int argc, char* args[] ) {

	// Simple Object Declaration

	using Foo = ctom::obj<
		ctom::key<"foo-int", int>,
		ctom::key<"foo-float", float>,
		ctom::key<"foo-double", double>
	>;

	ctom::print<Foo>(); // NOTE: FULLY STATIC! NO INSTANCE!

	// Simple Array Declaration

	using Barr = ctom::arr<6, int>;
	ctom::print<Barr>();

	// Nested Objects -> {Objects, Arrays}

	using Bar = ctom::obj<
		ctom::key<"bar-foo", Foo>,
		ctom::key<"bar-char", char>,
		ctom::key<"bar-barr", Barr>
	>;

	using Baz = ctom::obj<
		ctom::key<"baz-bar", Bar>,
		ctom::key<"baz-bool", bool>
	>;

	ctom::print<Baz>();

	// Nested Arrays -> {Arrays, Objects}

	using Maz = ctom::obj<
		ctom::key<"maz-char", char>
	>;

	using Marr = ctom::arr<3, Maz>;

	using MarrArr = ctom::arr<2, Marr>;

	ctom::print<MarrArr>();

	// Extended Object Declaration

	using FooExt = Foo::ext<
		ctom::key<"foo-ext-int", int>,
		ctom::key<"foo-ext-foo", Foo>
	>;

	ctom::print<FooExt>();

	// Extended Array Declaration

	using MarrExt = Marr::ext<2, Maz>;
	ctom::print<MarrExt>();

	return 0;

}
