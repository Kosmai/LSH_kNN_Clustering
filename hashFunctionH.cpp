#include <iostream>
#include <random>
#include "hashFunctionH.hpp"
#include "randGen.hpp"

HashFunctionH::HashFunctionH(){}

//Fill v with random doubles
HashFunctionH::HashFunctionH(int w, int dims) : w(w){
	t = getUniformRandomFloat()*w;
	for(int i = 0; i < dims; i++){
		this->v.push_back(getNormalRandom());
	}
}

HashFunctionH::~HashFunctionH(){

}

void HashFunctionH::printVec(){
	std::cout << "Vector v" << std::endl << "=================" << std::endl;
	for(auto i : this->v){
		std::cout << "|-- > " << i << std::endl;
	}
	std::cout << "=================" << std::endl;
}

//Return h(p) used in LSH
int HashFunctionH::computeH(std::vector<double>& p){
	return (std::inner_product(std::begin(this->v), std::end(this->v), std::begin(p), 0.0) + t)/w;
}
