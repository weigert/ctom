#include "../../src/ctom.hpp"

int main( int argc, char* args[] ) {

	// Simple Object Declaration

	struct Foo: ctom::obj<
		ctom::key<"foo-int", int>,
		ctom::key<"foo-float", float>,
		ctom::key<"foo-double", double>
	>{};

	ctom::print<Foo>(); // NOTE: FULLY STATIC! NO INSTANCE!

	// Simple Array Declaration

	struct Barr: ctom::seq<int, 6>{};
	ctom::print<Barr>();

	// Nested Objects -> {Objects, Arrays}

	struct Bar: ctom::obj<
		ctom::key<"bar-foo", Foo>,
		ctom::key<"bar-char", char>,
		ctom::key<"bar-barr", Barr>
	>{};

	struct Baz: ctom::obj<
		ctom::key<"baz-bar", Bar>,
		ctom::key<"baz-bool", bool>
	>{};

	ctom::print<Baz>();

	// Nested Arrays -> {Arrays, Objects}

	struct Maz: ctom::obj<
		ctom::key<"maz-char", char>
	>{};

	struct Marr: ctom::seq<Maz, 3>{};

	struct MarrArr: ctom::seq<Marr, 2>{};

	ctom::print<MarrArr>();

	// Extended Object Declaration

	struct FooExt: Foo::ext<
		ctom::key<"foo-ext-int", int>,
		ctom::key<"foo-ext-foo", Foo>
	>{};

	ctom::print<FooExt>();

	// Extended Array Declaration

	struct MarrExt: Marr::ext<
		ctom::ind<3, Maz>,
		ctom::ind<4, Maz>,
		ctom::ind<6, Maz>
	>{};

	ctom::print<MarrExt>();

	return 0;

}
