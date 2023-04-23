#include "../../src/ctom.hpp"
#include "../../src/yaml.hpp"
#include "../../src/json.hpp"

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct Arr: ctom::arr<
		ctom::ind<0, int>,
		ctom::ind<1, int>,
		ctom::ind<2, int>
	>{};

	struct Foo: ctom::obj<
		ctom::key<"foo-int", int>,
		ctom::key<"foo-float", float>,
		ctom::key<"foo-double", double>
	>{};

	struct Bar: ctom::obj<
		ctom::key<"bar-foo", Foo>,
		ctom::key<"bar-char", char>,
		ctom::key<"int-arr", Arr>
	>{};

	struct BarArr: ctom::arr<
		ctom::ind<0, Bar>,
		ctom::ind<1, Bar>
	>{};

	struct Root: ctom::obj <
		ctom::key<"int", int>,
		ctom::key<"float", float>,
		ctom::key<"double", double>,
		ctom::key<"bar", BarArr>
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
	std::cout<<ctom::yaml::emit<<root;
	std::cout<<ctom::json::emit<<root;

	return 0;

}
