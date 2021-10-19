#include <vector>
#include <iostream>
#include "hashFunctionH.hpp"

HashFunctionH::HashFunctionH(){
	v = {1, 2, 5, 7};
}

HashFunctionH::~HashFunctionH(){

}

void HashFunctionH::printVec(){
	for(auto i : this->v){
		std::cout << i << ' ';
	}
	std::cout << std::endl;
}