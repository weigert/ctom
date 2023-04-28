#include "../../src/ctom.hpp"
#include "../../src/yaml.hpp"

int main( int argc, char* args[] ) {

	// Type Definitions

	using Foo = ctom::obj<
		ctom::key<"foo-int", int>,
		ctom::key<"foo-float", float>,
		ctom::key<"foo-double", double>
	>;

	using Barr = ctom::arr<4, int>;


	using Bar = ctom::obj<
		ctom::key<"bar-foo", Foo>,
		ctom::key<"bar-char", char>,
		ctom::key<"bar-barr", Barr>
	>;

	using Baz = ctom::obj<
		ctom::key<"baz-bar", Bar>,
		ctom::key<"baz-bool", bool>
	>;

	using Maz = ctom::obj<
		ctom::key<"maz-char", char>
	>;

	using Marr = ctom::arr<3, Maz>;

	using MarrArr = ctom::arr<2, Marr>;

	// Simple Object Implementation

	struct Foo_Impl: Foo {
		int x = 5;
		float y = 0.5f;
		double z = 0.25;
		Foo_Impl(){
			this->val<"foo-int">() = x;
			this->val<"foo-float">() = y;
			this->val<"foo-double">() = z;
		}
	} foo_impl;

	std::cout << ctom::yaml::emit << foo_impl;

	foo_impl.get<"foo-int">() = 2;
	foo_impl.get<"foo-float">() = 0.25f;
	foo_impl.z = 3;

	std::cout << ctom::yaml::emit << foo_impl;

	// Simple Array Implementation

	struct Barr_Impl: Barr {
		int a = 0;
		int b = 1;
		int c = 2;
		int d = 3;
		Barr_Impl(){
			this->val<0>() = a;
			this->val<1>() = b;
			this->val<2>() = c;
			this->val<3>() = d;
		}
	} barr_impl;

	std::cout << ctom::yaml::emit << barr_impl;

	barr_impl.a = 3;	// direct assignment
	barr_impl.b = 2;
	barr_impl.c = 1;
	barr_impl.d = 0;

	Barr_Impl new_barr_impl;

	std::cout << ctom::yaml::emit << barr_impl;
	std::cout << ctom::yaml::emit << new_barr_impl;

	// Nested Objects -> {Objects, Arrays}

	struct Bar_Impl: Bar {
		Foo_Impl foo;
		char c = 'x';
		Barr_Impl bar;
		Bar_Impl(){
			this->val<"bar-foo">() = foo;
			this->val<"bar-char">() = c;
			this->val<"bar-barr">() = bar;
		}
	};

	struct Baz_Impl: Baz {
		Bar_Impl bar_impl;
		bool b = true;
		Baz_Impl(){
			this->val<"baz-bar">() = bar_impl;
			this->val<"baz-bool">() = b;
		}
	} baz_impl;

	std::cout << ctom::yaml::emit << baz_impl;

	// Nested Arrays -> {Arrays, Objects}

	struct Maz_Impl: Maz {
		char c = ' ';
		Maz_Impl(){
			this->val<"maz-char">() = c;
		}
	};

	struct Marr_Impl: Marr {
		Maz_Impl maz0, maz1, maz2;
		Marr_Impl(){
			this->val<0>() = maz0;
			this->val<1>() = maz1;
			this->val<2>() = maz2;
		}
	};

	struct MarrArr_Impl: MarrArr {
		Marr_Impl marr0, marr1;
		MarrArr_Impl(){
			this->val<0>() = marr0;
			this->val<1>() = marr1;	
		}
	} marrarr_impl;

	marrarr_impl.marr0.maz0.c = 'a';
	marrarr_impl.marr0.maz1.c = 'b';
	marrarr_impl.marr0.maz2.c = 'c';
	marrarr_impl.marr1.maz0.c = 'd';
	marrarr_impl.marr1.maz1.c = 'e';
	marrarr_impl.marr1.maz2.c = 'f';

	std::cout << ctom::yaml::emit << marrarr_impl;

	marrarr_impl.get<0>().get<0>().get<"maz-char">() = 'g';
	marrarr_impl.get<0>().get<1>().get<"maz-char">() = 'h';
	marrarr_impl.get<0>().get<2>().get<"maz-char">() = 'i';
	marrarr_impl.get<1>().get<0>().get<"maz-char">() = 'j';
	marrarr_impl.get<1>().get<1>().get<"maz-char">() = 'k';
	marrarr_impl.get<1>().get<2>().get<"maz-char">() = 'l';

	std::cout << ctom::yaml::emit << marrarr_impl;

	return 0;

}
