#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "hashFunctionG.hpp"
#include "randGen.hpp"

int main(){
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	setRandomSeed(seed);
	std::vector<double> p = {1, 1, 1};
	HashFunctionG* g1 = new HashFunctionG(4, 2, 3);
	std::cout << g1->computeG(p);

	delete g1;
	return 0;
}