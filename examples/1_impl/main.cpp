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
		int& x 		= Foo::val<"foo-int">(1);
		float& y 	= Foo::val<"foo-float">(0.5f);
	//	double& z 	= Foo::val<"foo-double">(0.25);
	} foo_impl;

	ctom::print(foo_impl);

	foo_impl.get<"foo-int">() = 2;
	foo_impl.get<"foo-float">() = 0.25f;

	ctom::print(foo_impl);

	// Simple Array Implementation

	using Barr = ctom::arr<int, 4>;

	struct Barr_Impl: Barr {
		int& a = Barr::val<0>(0);
		int& b = Barr::val<1>(1);
		int& c = Barr::val<2>(2);
		int& d = Barr::val<3>(3);
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
		Foo_Impl& foo 		= Bar::obj<"bar-foo", Foo_Impl>();
		char& c     		= Bar::val<"bar-char">('x');
		Barr_Impl& barr 	= Bar::arr<"bar-barr", Barr_Impl>();
	};

	struct Baz_Impl: Baz {
		Bar_Impl& bar_impl 	= Baz::obj<"baz-bar", Bar_Impl>();
		bool& b 			= Baz::val<"baz-bool">(true);
	} baz_impl;

	ctom::print(baz_impl);

	// Nested Arrays -> {Arrays, Objects}

	using Maz = ctom::obj<
		ctom::key<"maz-char", char>
	>;

	using Marr = ctom::arr<Maz, 3>;

	using MarrArr = ctom::arr<Marr, 2>;

	struct Maz_Impl: Maz {
		char& c 	= Maz::val<"maz-char">(' ');
	};

	struct Marr_Impl: Marr {
		Maz_Impl& maz0 	= Marr::obj<0, Maz_Impl>();
		Maz_Impl& maz1 	= Marr::obj<1, Maz_Impl>();
		Maz_Impl& maz2 	= Marr::obj<2, Maz_Impl>();
	};

	struct MarrArr_Impl: MarrArr {
		Marr_Impl& marr0 	= MarrArr::arr<0, Marr_Impl>();
		Marr_Impl& marr1 	= MarrArr::arr<1, Marr_Impl>();	
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
