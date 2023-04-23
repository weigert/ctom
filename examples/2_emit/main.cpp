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

	using Root = ctom::obj<
		ctom::key<"int", int>,
		ctom::key<"float", float>,
		ctom::key<"double", double>,
		ctom::key<"bar", BarArr>
	>;

	// Concrete Implementations

	struct Arr_Impl: Arr {
		int arr[3] = {0};
		Arr_Impl(){
			this->val<0>() = arr[0];
			this->val<1>() = arr[1];
			this->val<2>() = arr[2];
		}
	};

	struct Foo_Impl: Foo {
		int a = 10;
		float b = 5.0f;
		double c = 2.5;
		Foo_Impl(){
			this->val<"foo-int">() = a;
			this->val<"foo-float">() = b;
			this->val<"foo-double">() = c;
		}
	};

	struct Bar_Impl: Bar {
		Foo_Impl foo;
		char x = 'y';
		Arr_Impl arr;
		Bar_Impl(){
			this->val<"bar-foo">() = foo;
			this->val<"bar-char">() = x;
			this->val<"int-arr">() = arr;
		}
	};

	struct BarArr_Impl: BarArr {
		Bar_Impl b[2];
		BarArr_Impl(){
			this->val<0>() = b[0];
			this->val<1>() = b[1];
		}
	};

	struct Root_Impl: Root {
		BarArr_Impl bar;
		Root_Impl(){
			this->val<"bar">() = bar;
		}
	};

	// Single Instantiation

	Root_Impl root;
	std::cout<<ctom::yaml::emit<<root;
	std::cout<<ctom::json::emit<<root;

	return 0;

}
