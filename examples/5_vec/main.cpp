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

// Owning Derived-Class Interface
// 	Owns the data!

template<typename T>
struct vec3_t: vec3<T>, ctom::arr<3, T>{
	vec3_t():ctom::arr<3, T>(x, y, z){};
	using vec3<T>::x;
	using vec3<T>::y;
	using vec3<T>::z;
};

// Forwarding Only Interface

template<typename T>
struct vec3_forward: ctom::arr<3, T>{
	vec3_forward(vec3<T>& vec)
	:ctom::arr<3, T>(vec.x, vec.y, vec.z){};
};

int main( int argc, char* args[] ) {

	std::ifstream yaml_file;

	// Owning, Nested Derived Type

	vec3_t<vec3_t<int>> owning_vec;

	yaml_file.open("config.yaml");
	if(yaml_file.is_open()){
	
		try {
			yaml_file >> ctom::yaml::parse >> owning_vec;
			std::cout << ctom::yaml::emit << owning_vec;
		} catch(ctom::yaml::exception e){
			std::cout<<"Failed to parse config.yaml: "<<e.what()<<std::endl;
		}

		yaml_file.close();

	}

	// Non-Owning Reference Parser
	//	Note: Won't work with nested vecs,
	// 	because the nested vec-type won't be parsable

	vec3<float> vec;
	vec3_forward ref(vec);

	yaml_file.open("config2.yaml");
	if(yaml_file.is_open()){
	
		try {
			yaml_file >> ctom::yaml::parse >> ref;
			std::cout << ctom::yaml::emit << ref;
		} catch(ctom::yaml::exception e){
			std::cout<<"Failed to parse config.yaml: "<<e.what()<<std::endl;
		}

		yaml_file.close();

	}


	return 0;

}
