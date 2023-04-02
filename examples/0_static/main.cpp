#include "../../src/ctom.hpp"

int main( int argc, char* args[] ) {

	struct Foo: ctom::obj<
		ctom::key::val<"foo-int", int>,
		ctom::key::val<"foo-float", float>,
		ctom::key::val<"foo-double", double>
	>{};

	struct Foo_Ext: Foo::ext<
		ctom::key::val<"foo-ext-int", int>,
		ctom::key::obj<"foo-ext-foo", Foo>
	>{};

	struct Bar: ctom::obj<
		ctom::key::obj<"bar-foo-ext", Foo_Ext>,
		ctom::key::val<"bar-char", char>
	>{};

	// This needs to be constructed more easily!
	//	For instance, through expansion!

	struct Bar_arr: ctom::arr<
		ctom::ind::obj<0, Bar>,
		ctom::ind::obj<1, Bar>
	>{};

	struct Root: ctom::obj<
		ctom::key::val<"root-int", int>,
		ctom::key::arr<"root-bar-array", Bar_arr>,
		ctom::key::obj<"root-bar", Bar>,
		ctom::key::val<"root-int-2", int>
	>{};

	ctom::print<Root>();

	return 0;

}
