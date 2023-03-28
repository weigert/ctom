#include "../ctom.hpp"

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct A_model: ctom::obj_impl<
		ctom::val<"sub-sub-int", int>,
		ctom::val<"sub-sub-float", float>,
		ctom::val<"sub-sub-double", double>
	>{};

		// Abstract Model Extension

	struct A_extended: A_model::ext<
		ctom::val<"sub-sub-int-2", int>,
		ctom::obj<"sub-sub-obj-ext", A_model>
	>{};

	struct B_model: ctom::obj_impl<
		ctom::obj<"sub-obj", A_extended>,
		ctom::val<"sub-char", char>
	>{};

	struct T_model: ctom::obj_impl <
		ctom::val<"valA", int>,
		ctom::arr<"array", B_model>,
		ctom::obj<"object", B_model>,
		ctom::val<"valB", int>
	>{};

	std::cout<<"(static)"<<std::endl;
	ctom::print<T_model>();

	// Concrete Model Translation

	struct A: A_extended {
		int a;
		float b;
		double c;
	};

	struct B: B_model {
		A a = B::obj<"sub-obj">(A{});
		char c = B::val<"sub-char">('z');
	};

	struct T: T_model {
		int t = T::val<"valA">(10);
		int a = T::val<"valA">(5);
		B b = T::obj<"object">(B{});
	} Tval;

	std::cout<<std::endl<<"(instance)"<<std::endl;
	ctom::print(Tval);

	return 0;

}
