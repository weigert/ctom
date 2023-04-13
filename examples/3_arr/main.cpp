#include "../../src/ctom.hpp"
#include "../../src/yaml.hpp"
#include <vector>
#include <set>

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct Foo: ctom::obj<
		ctom::key::val<"foo-int", int>
	>{};

	struct Bar: ctom::arr<
		ctom::ind::obj<0, Foo>,
		ctom::ind::obj<1, Foo>,
		ctom::ind::obj<2, Foo>
	>{};

	struct Arr: ctom::arr<
		ctom::ind::arr<0, Bar>,
		ctom::ind::arr<1, Bar>
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

	arr.get<0>().get<0>().get<"foo-int">() = 7;
	arr.get<0>().get<1>().get<"foo-int">() = 8;
	arr.get<0>().get<2>().get<"foo-int">() = 9;
	arr.get<1>().get<0>().get<"foo-int">() = 0;
	arr.get<1>().get<1>().get<"foo-int">() = 1;
	arr.get<1>().get<2>().get<"foo-int">() = 2;
	ctom::print(arr);

	std::cout<<ctom::yaml::emit<<arr;

	return 0;

}
