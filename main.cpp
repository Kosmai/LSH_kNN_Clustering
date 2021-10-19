#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "hashFunctionH.hpp"

int main(){
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::vector<double> p = {1, 1, 1, 1, 1};
	HashFunctionH* h1 = new HashFunctionH(4, 5, seed);
	h1->printVec();
	std::cout << h1->computeH(p);
	
	delete h1;
	return 0;
}