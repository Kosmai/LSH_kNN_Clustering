#include <iostream>
#include <random>
#include "../inc/hashFunctionH.hpp"
#include "../inc/randGen.hpp"

//Fill v with random doubles
HashFunctionH::HashFunctionH(int w, int dims) : w(w){
	t = getUniformRandomFloat()*w; //Generate t = random double in [0,w]
	for(int i = 0; i < dims; i++){
		this->v.push_back(getNormalRandom()); //Append normal dist. double to the vector
	}
}

//Prints the vector that represent this H function
void HashFunctionH::printVec(){
	std::cout << "Vector v" << std::endl << "=================" << std::endl;
	for(auto i : this->v){
		std::cout << "|-- > " << i << std::endl;
	}
	std::cout << "=================" << std::endl;
}

//Return   h(p) = (p dot v + t) / w   used in LSH 
int HashFunctionH::computeH(std::vector<double>& p){
	return (std::inner_product(std::begin(this->v), std::end(this->v), std::begin(p), 0.0) + t)/w;
}
