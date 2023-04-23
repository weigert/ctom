#include "../../src/ctom.hpp"
#include "../../src/yaml.hpp"
#include "../../src/json.hpp"

int main( int argc, char* args[] ) {

	// Abstract Object Models

	using Arr = ctom::arr<3, int>;

	using Foo = ctom::obj<
		ctom::key<"foo-int", int>,
		ctom::key<"foo-float", float>,
		ctom::key<"foo-double", double>
	>;

	using Bar = ctom::obj<
		ctom::key<"bar-foo", Foo>,
		ctom::key<"bar-char", char>,
		ctom::key<"int-arr", Arr>
	>;

	using BarArr = ctom::arr<2, Bar>;

	using Root = ctom::obj <
		ctom::key<"int", int>,
		ctom::key<"float", float>,
		ctom::key<"double", double>,
		ctom::key<"bar", BarArr>
	>;

	// Concrete Implementations

	struct Arr_Impl: Arr {
		int arr[3];
		Arr_Impl(){
			Arr::val<0>(arr[0]) = 9;
			Arr::val<1>(arr[1]) = 7;
			Arr::val<2>(arr[2]) = 5;
		}
	};

	struct Foo_Impl: Foo {
		int a;
		float b;
		double c;
		Foo_Impl(){
			Foo::val<"foo-int">(a) = 10;
			Foo::val<"foo-float">(b) = 5.0f;
			Foo::val<"foo-double">(c) = 2.5;
		}
	};

	struct Bar_Impl: Bar {
		Foo_Impl foo;
		char x;
		Arr_Impl arr;
		Bar_Impl(){
			Bar::obj<"bar-foo">(foo);
			Bar::val<"bar-char">(x) = 'y';
			Bar::arr<"int-arr">(arr);
		}
	};

	struct BarArr_Impl: BarArr {
		Bar_Impl b[2];
		BarArr_Impl(){
			BarArr::obj<0>(b[0]);
			BarArr::obj<1>(b[1]);
		}
	};

	struct Root_Impl: Root {
		BarArr_Impl bar;
		Root_Impl(){
			Root::arr<"bar">(bar);
		}
	};

	// Single Instantiation

	Root_Impl root;
	std::cout<<ctom::yaml::emit<<root;
	std::cout<<ctom::json::emit<<root;

	return 0;

}
