#include "../../ctom.hpp"

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct Foo: ctom::obj_impl<
		ctom::val<"foo-int", int>,
		ctom::val<"foo-float", float>,
		ctom::val<"foo-double", double>
	>{};

	// Abstract Model Extension
	//	With reference to non-extended self

	struct Foo_Ext: Foo::ext<
		ctom::val<"foo-ext-int", int>,
		ctom::obj<"foo-ext-foo", Foo>
	>{};

	// Nested Object of Extension

	struct Bar: ctom::obj_impl<
		ctom::obj<"bar-foo-ext", Foo_Ext>,
		ctom::val<"bar-char", char>
	>{};

	struct Root: ctom::obj_impl <
		ctom::val<"root-int", int>,
		ctom::arr<"root-bar-array", Bar>,
		ctom::obj<"root-bar", Bar>,
		ctom::val<"root-int-2", int>
	>{};

	ctom::print<Root>();

	return 0;

}
