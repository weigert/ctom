#include "../../src/ctom.hpp"
#include "../../src/yaml.hpp"

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct Arr: ctom::arr<
		ctom::ind::val<0, int>,
		ctom::ind::val<1, int>,
		ctom::ind::val<2, int>
	>{};

	struct Foo: ctom::obj<
		ctom::key::val<"foo-int", int>,
		ctom::key::val<"foo-float", float>,
		ctom::key::val<"foo-double", double>
	>{};

	struct Bar: ctom::obj<
		ctom::key::obj<"bar-foo", Foo>,
		ctom::key::val<"bar-char", char>,
		ctom::key::arr<"int-arr", Arr>
	>{};

	struct BarArr: ctom::arr<
		ctom::ind::obj<0, Bar>,
		ctom::ind::obj<1, Bar>
	>{};

	struct Root: ctom::obj <
		ctom::key::val<"int", int>,
		ctom::key::val<"float", float>,
		ctom::key::val<"double", double>,
		ctom::key::arr<"bar", BarArr>
	>{};

	// Concrete Implementations

	struct Arr_Impl: Arr {
		int& a = Arr::val<0, int>(9);
		int& b = Arr::val<1, int>(7);
		int& c = Arr::val<2, int>(5);
	};

	struct Foo_Impl: Foo {
		int& a 	 	= Foo::val<"foo-int">(10);
		float& b  	= Foo::val<"foo-float">(5.0f);
		double& c 	= Foo::val<"foo-double">(2.5);
	};

	struct Bar_Impl: Bar {
		Foo_Impl& foo	= Bar::obj<"bar-foo", Foo_Impl>();
		char& x 		= Bar::val<"bar-char">('y');
		Arr_Impl& arr 	= Bar::arr<"int-arr", Arr_Impl>();
	};

	struct BarArr_Impl: BarArr {
		Bar_Impl& b0 	= BarArr::obj<0, Bar_Impl>();
		Bar_Impl& b1 	= BarArr::obj<1, Bar_Impl>();
	};

	struct Root_Impl: Root {
		BarArr_Impl& bar = Root::arr<"bar", BarArr_Impl>();
	};

	// Single Instantiation

	Root_Impl root;
	std::cout<<ctom::yaml::emit<<root;//<<root;

	return 0;

}
