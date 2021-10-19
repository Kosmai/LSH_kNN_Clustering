#include <iostream>
#include <vector>
#include "hashFunctionH.hpp"

int main(){
	HashFunctionH* h1 = new HashFunctionH;
	h1->printVec();


	delete h1;
	return 0;
}