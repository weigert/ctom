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

// Forwarding Only Interface

// Parse-Rule

template<typename T>
struct vec3_p: ctom::arr<3, T>{
	vec3_p(vec3<T>& vec)
	:ctom::arr<3, T>(vec.x, vec.y, vec.z){};
};

template<typename T>
struct ctom::rule<vec3<T>>{
	typedef vec3_p<T> type;
};

// Other Well-Known Type

struct my_type {
	vec3<int> x;
	std::string some_text;
};

using my_type_t = ctom::obj<
	ctom::key<"some_vec", vec3<int>>,
	ctom::key<"some_text", std::string>
>;

struct my_type_p: my_type_t {
	my_type_p(my_type& t)
	:my_type_t(t.x, t.some_text){};
};

template<>
struct ctom::rule<my_type> {
	typedef my_type_p type;
};

int main( int argc, char* args[] ) {

	std::ifstream yaml_file;

	vec3<float> vec;

	yaml_file.open("config2.yaml");
	if(yaml_file.is_open()){
	
		try {
			yaml_file >> ctom::yaml::parse >> vec;
			std::cout << ctom::yaml::emit << vec;
		} catch(ctom::yaml::exception e){
			std::cout<<"Failed to parse config.yaml: "<<e.what()<<std::endl;
		}

		yaml_file.close();

	}

	vec3<vec3<float>> nvec;

	yaml_file.open("config.yaml");
	if(yaml_file.is_open()){
	
		try {
			yaml_file >> ctom::yaml::parse >> nvec;
			std::cout << ctom::yaml::emit << nvec;
		} catch(ctom::yaml::exception e){
			std::cout<<"Failed to parse config.yaml: "<<e.what()<<std::endl;
		}

		yaml_file.close();

	}

	my_type m;

	yaml_file.open("config3.yaml");
	if(yaml_file.is_open()){
	
		try {
			yaml_file >> ctom::yaml::parse >> m;
			std::cout << ctom::yaml::emit << m;
		} catch(ctom::yaml::exception e){
			std::cout<<"Failed to parse config.yaml: "<<e.what()<<std::endl;
		}

		yaml_file.close();

	}


	return 0;

}
