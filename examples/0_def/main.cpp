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

	struct Barr: ctom::arr<
		ctom::ind<0, int>,
		ctom::ind<1, int>,
		ctom::ind<2, int>,
		ctom::ind<3, int>
	>{};

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

	struct Marr: ctom::arr<
		ctom::ind<0, Maz>,
		ctom::ind<1, Maz>,
		ctom::ind<2, Maz>
	>{};

	struct MarrArr: ctom::arr<
		ctom::ind<0, Marr>,
		ctom::ind<1, Marr>
	>{};

	ctom::print<MarrArr>();

	/*

	// Extended Object Declaration

	struct FooExt: Foo::ext<
		ctom::key::val<"foo-ext-int", int>,
		ctom::key::obj<"foo-ext-foo", Foo>
	>{};

	ctom::print<FooExt>();

	// Extended Array Declaration

	struct MarrExt: Marr::ext<
		ctom::ind::obj<3, Maz>,
		ctom::ind::obj<4, Maz>,
		ctom::ind::obj<5, Maz>
	>{};

	ctom::print<MarrExt>();
	
	*/

	return 0;

}
