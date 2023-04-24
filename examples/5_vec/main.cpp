#include "../../src/ctom.hpp"
#include "../../src/yaml.hpp"
#include <vector>
#include <set>
#include <fstream>

// Templated Well-Known Type

template<typename T>
struct vec3 {
	T x;
	T y;
	T z;
};

// CTOM Interface
// 	Makes this type parseable however you like
//	...make sure to implement your copy and move constructors
// 	unlike me

template<typename T>
struct vec3_t: vec3<T>, ctom::arr<3, T>{
	vec3_t():ctom::arr<3, T>(x, y, z){};
	using vec3<T>::x;
	using vec3<T>::y;
	using vec3<T>::z;
};

int main( int argc, char* args[] ) {

	// Object Containing Different Vector-Types

	using Maz_t = ctom::obj<
		ctom::key<"vecA", vec3_t<int>>,
		ctom::key<"vecB", vec3_t<float>>,
		ctom::key<"vecC", vec3_t<double>>
	>;

	// Implementation and Compound-Assignment to Keys

	struct Maz: Maz_t {
		Maz():Maz_t(a, b, c){};
		vec3_t<int> a;
		vec3_t<float> b;
		vec3_t<double> c;
	} maz;

	// Parse and Emit

	std::ifstream yaml_file("config.yaml");
	if(yaml_file.is_open()){
	
		try {
			yaml_file >> ctom::yaml::parse >> maz;
			std::cout << ctom::yaml::emit << maz;
		} catch(ctom::yaml::exception e){
			std::cout<<"Failed to parse config.yaml: "<<e.what()<<std::endl;
		}

		yaml_file.close();

	}

	// Vector of Vector? Why of course!

	vec3_t<vec3_t<int>> vec_of_vec;

	yaml_file.open("config2.yaml");
	if(yaml_file.is_open()){
	
		try {
			yaml_file >> ctom::yaml::parse >> vec_of_vec;
			std::cout << ctom::yaml::emit << vec_of_vec;
		} catch(ctom::yaml::exception e){
			std::cout<<"Failed to parse config.yaml: "<<e.what()<<std::endl;
		}

		yaml_file.close();

	}

	return 0;

}
