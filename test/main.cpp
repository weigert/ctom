#include "../ctom.hpp"

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct A: ctom::obj_impl<
		ctom::val<"sub-sub-int", int>,
		ctom::val<"sub-sub-float", float>,
		ctom::val<"sub-sub-double", double>
	>{};

	struct B: ctom::obj_impl<
		ctom::obj<"sub-obj", A>,
		ctom::val<"sub-char", char>
	>{};

	struct T: ctom::obj_impl <
		ctom::val<"valA", int>,
		ctom::arr<"array", int>,
		ctom::obj<"object", B>,
		ctom::val<"valB", int>
	>{};

	std::cout<<"(static)"<<std::endl;
	ctom::print<T>();

	// Concrete Model Translation

	struct B_derived: B {
		char c = B::val<"sub-char">('y');
	};

	struct T_derived: T {
		int t = T::val<"valA">(10);
		int a;
		B b = T::obj<"object">(B{});
	} Tval;

	std::cout<<std::endl<<"(instance)"<<std::endl;
	ctom::print(Tval);

	/*
			Now I need to write an actual proper printer...
			And arrays need to work too...
			And I need to get the type copying to also work,
			i.e. a parser thingy.
	*/


	return 0;

}
