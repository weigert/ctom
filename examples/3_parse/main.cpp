#include "../../src/ctom.hpp"
#include "../../src/yaml.hpp"
#include "../../src/json.hpp"

#include <fstream>

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct Arr: ctom::arr<int, 3>{};

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

	struct BarArr: ctom::arr<Bar, 2>{};

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
		int& x = Root::val<"int">(6);
	};

	// Single Instantiation

	Root_Impl root;

	std::ifstream yaml_file("config.yaml");
	if(yaml_file.is_open()){
	
		try {
			yaml_file >> ctom::yaml::parse >> root;
			std::cout << ctom::yaml::emit << root;
		} catch(ctom::yaml::exception e){
			std::cout<<"Failed to parse config.yaml: "<<e.what()<<std::endl;
		}

		yaml_file.close();

	}

	return 0;

}
