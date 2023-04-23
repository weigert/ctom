#include "../../src/ctom.hpp"
#include <vector>
#include <set>

int main( int argc, char* args[] ) {

	// Simple Object Implementation

	using Foo = ctom::obj<
		ctom::key<"foo-int", int>,
		ctom::key<"foo-float", float>,
		ctom::key<"foo-double", double>
	>;

	struct Foo_Impl: Foo {
		Foo_Impl(){
			Foo::val<"foo-int">(x) = 5;
			Foo::val<"foo-float">(y) = 0.5f;
			Foo::val<"foo-double">(z) = 0.25;
		}
		int x;
		float y;
		double z;
	} foo_impl;

	ctom::print(foo_impl);

	foo_impl.get<"foo-int">() = 2;
	foo_impl.get<"foo-float">() = 0.25f;

	foo_impl.x = 3;

	ctom::print(foo_impl);

	// Simple Array Implementation

	using Barr = ctom::arr<4, int>;

	struct Barr_Impl: Barr {
		Barr_Impl(){
			Barr::val<0>(a) = 0;
			Barr::val<1>(b) = 1;
			Barr::val<2>(c) = 2;
			Barr::val<3>(d) = 3;
		}
		int a, b, c, d;
	} barr_impl;

	ctom::print(barr_impl);

	barr_impl.a = 3;	// direct assignment
	barr_impl.b = 2;
	barr_impl.c = 1;
	barr_impl.d = 0;

	Barr_Impl new_barr_impl;

	ctom::print(barr_impl);
	ctom::print(new_barr_impl);

	// Nested Objects -> {Objects, Arrays}

	using Bar = ctom::obj<
		ctom::key<"bar-foo", Foo>,
		ctom::key<"bar-char", char>,
		ctom::key<"bar-barr", Barr>
	>;

	using Baz = ctom::obj<
		ctom::key<"baz-bar", Bar>,
		ctom::key<"baz-bool", bool>
	>;

	struct Bar_Impl: Bar {
		Bar_Impl(){
			Bar::obj<"bar-foo">(foo);
			Bar::val<"bar-char">(c) = 'x';
			Bar::arr<"bar-barr">(bar);
		}
		Foo_Impl foo;
		char c;
		Barr_Impl bar;
	};


	struct Baz_Impl: Baz {
		Baz_Impl(){
			Baz::obj<"baz-bar">(bar_impl);
			Baz::val<"baz-bool">(b) = true;
		}
		Bar_Impl bar_impl;
		bool b;
	} baz_impl;

	ctom::print(baz_impl);


	// Nested Arrays -> {Arrays, Objects}

	using Maz = ctom::obj<
		ctom::key<"maz-char", char>
	>;

	using Marr = ctom::arr<3, Maz>;

	using MarrArr = ctom::arr<2, Marr>;

	struct Maz_Impl: Maz {
		Maz_Impl(){
			Maz::val<"maz-char">(c) = ' ';
		}
		char c;
	};

	struct Marr_Impl: Marr {
		Marr_Impl(){
			Marr::obj<0>(maz0);
			Marr::obj<1>(maz1);
			Marr::obj<2>(maz2);
		}
		Maz_Impl maz0, maz1, maz2;
	};

	struct MarrArr_Impl: MarrArr {
		Marr_Impl marr0, marr1;
		MarrArr_Impl(){
			MarrArr::arr<0>(marr0);
			MarrArr::arr<1>(marr1);	
		}
	} marrarr_impl;

	marrarr_impl.marr0.maz0.c = 'a';
	marrarr_impl.marr0.maz1.c = 'b';
	marrarr_impl.marr0.maz2.c = 'c';
	marrarr_impl.marr1.maz0.c = 'd';
	marrarr_impl.marr1.maz1.c = 'e';
	marrarr_impl.marr1.maz2.c = 'f';

	ctom::print(marrarr_impl);

	marrarr_impl.get<0>().get<0>().get<"maz-char">() = 'g';
	marrarr_impl.get<0>().get<1>().get<"maz-char">() = 'h';
	marrarr_impl.get<0>().get<2>().get<"maz-char">() = 'i';
	marrarr_impl.get<1>().get<0>().get<"maz-char">() = 'j';
	marrarr_impl.get<1>().get<1>().get<"maz-char">() = 'k';
	marrarr_impl.get<1>().get<2>().get<"maz-char">() = 'l';

	ctom::print(marrarr_impl);

	/*

	// Extended Object Implementation

	struct FooExt: Foo::ext<
		ctom::key::val<"foo-ext-int", int>,
		ctom::key::obj<"foo-ext-foo", Foo>
	>{};

	// Extended Array Implementation

	struct MarrExt: Marr::ext<
		ctom::ind::obj<3, Maz>,
		ctom::ind::obj<4, Maz>,
		ctom::ind::obj<5, Maz>
	>{};

	*/

	return 0;

}
