#include <iostream>
#include <vector>
#include <random>
#include "hashFunctionH.hpp"

//Generator for normal distribution
static std::default_random_engine generator;
static std::normal_distribution<double> distribution(0.0,1.0);

HashFunctionH::HashFunctionH(int w, int dims, int seed) : w(w){
	generator.seed(seed);
	for(int i = 0; i < dims; i++){
		this->v.push_back(this->getRandom());
	}
}

HashFunctionH::~HashFunctionH(){

}

double HashFunctionH::getRandom(){
	return distribution(generator);
}

void HashFunctionH::printVec(){\
	std::cout << "Vector v" << std::endl << "=================" << std::endl;
	for(auto i : this->v){
		std::cout << "|-- > " << i << std::endl;
	}
	std::cout << "=================" << std::endl;
}

int HashFunctionH::computeH(std::vector<double>& p){
	float t = (float)this->getRandom();
	return (std::inner_product(std::begin(this->v), std::end(this->v), std::begin(p), 0.0) + t)/w;
}
