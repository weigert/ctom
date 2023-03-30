#include "../../src/ctom.hpp"

int main( int argc, char* args[] ) {

	struct Foo: ctom::obj_impl<
		ctom::val<"foo-int", int>,
		ctom::val<"foo-int", int>,
		ctom::val<"foo-float", float>,
		ctom::val<"foo-double", double>
	>{};

	struct Foo_Ext: Foo::ext<
		ctom::val<"foo-ext-int", int>,
		ctom::obj<"foo-ext-foo", Foo>
	>{};

	struct Bar: ctom::obj_impl<
		ctom::obj<"bar-foo-ext", Foo_Ext>,
		ctom::val<"bar-char", char>
	>{};

	// This needs to be constructed more easily!
	//	For instance, through expansion!

	struct Bar_arr: ctom::arr_impl<
		ctom::_obj<0, Bar>,
		ctom::_obj<1, Bar>
	>{};

	struct Root: ctom::obj_impl <
		ctom::val<"root-int", int>,
		ctom::arr<"root-bar-array", Bar_arr>,
		ctom::obj<"root-bar", Bar>,
		ctom::val<"root-int-2", int>
	>{};



	/*
	// Abstract Object Models



	// Abstract Model Extension
	//	With reference to non-extended self



	// Nested Object of Extension



	*/
	ctom::print<Root>();

	return 0;

}
