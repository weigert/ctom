#include "../../src/ctom.hpp"
#include "../../src/yaml.hpp"
#include "../../src/json.hpp"

#include <fstream>

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
		int arr[3] = {9, 7, 5};
		Arr_Impl(){
			this->ind<0>() = arr[0];
			this->ind<1>() = arr[1];
			this->ind<2>() = arr[2];			
		}
	};

	struct Foo_Impl: Foo {
		int a = 10;
		float b = 5.0f;
		double c = 2.5;
		Foo_Impl(){
			this->key<"foo-int">() = a;
			this->key<"foo-float">() = b;
			this->key<"foo-double">() = c;
		}
	};

	struct Bar_Impl: Bar {
		Foo_Impl foo;
		char x = 'y';
		Arr_Impl arr;
		Bar_Impl(){
			this->key<"bar-foo">() = foo;
			this->key<"bar-char">() = x;
			this->key<"int-arr">() = arr;
		}
	};

	struct BarArr_Impl: BarArr {
		Bar_Impl b[2];
		BarArr_Impl(){	
			this->ind<0>() = b[0];
			this->ind<1>() = b[1];
		}
	};

	struct Root_Impl: Root {
		BarArr_Impl bar;
		int x = 6;
		Root_Impl(){
			this->key<"bar">() = bar;
			this->key<"int">() = x;
		}
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
