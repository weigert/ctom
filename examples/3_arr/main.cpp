#include "../../src/ctom.hpp"
#include <vector>
#include <set>

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct Foo: ctom::obj_impl<
		ctom::val<"foo-int", int>
	>{};

	struct Bar: ctom::arr_impl<
		ctom::_obj<0, Foo>,
		ctom::_obj<1, Foo>,
		ctom::_obj<2, Foo>
	>{};

	struct Arr: ctom::arr_impl<
		ctom::_arr<0, Bar>,
		ctom::_arr<1, Bar>
	>{};

	// Implementations
	struct Foo_Impl: Foo {
		int& x = Foo::val<"foo-int">(0);
	};

	struct Bar_Impl: Bar {
		Foo_Impl& fooA = Bar::obj<0, Foo_Impl>();
		Foo_Impl& fooB = Bar::obj<1, Foo_Impl>();
		Foo_Impl& fooC = Bar::obj<2, Foo_Impl>();
	};

	struct Arr_Impl: Arr {
		Bar_Impl& barA = Arr::arr<0, Bar_Impl>();
		Bar_Impl& barB = Arr::arr<1, Bar_Impl>();
	} arr;

	ctom::print(arr);


	// Modify and Print

	arr.barA.fooA.x = 1;
	arr.barA.fooB.x = 2;
	arr.barA.fooC.x = 3;
	arr.barB.fooA.x = 4;
	arr.barB.fooB.x = 5;
	arr.barB.fooC.x = 6;
	ctom::print(arr);

	arr[ctom::ind_impl<0>{}][ctom::ind_impl<0>{}]["foo-int"_key] = 7;
	arr[ctom::ind_impl<0>{}][ctom::ind_impl<1>{}]["foo-int"_key] = 8;
	arr[ctom::ind_impl<0>{}][ctom::ind_impl<2>{}]["foo-int"_key] = 9;
	arr[ctom::ind_impl<1>{}][ctom::ind_impl<0>{}]["foo-int"_key] = 0;
	arr[ctom::ind_impl<1>{}][ctom::ind_impl<1>{}]["foo-int"_key] = 1;
	arr[ctom::ind_impl<1>{}][ctom::ind_impl<2>{}]["foo-int"_key] = 2;
	ctom::print(arr);

	return 0;

}
