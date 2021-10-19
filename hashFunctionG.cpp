#include <iostream>
#include "hashFunctionG.hpp"
#include "hashFunctionH.hpp"
#include "randGen.hpp"

//Initialize k h functions and k "r" values.
HashFunctionG::HashFunctionG(int k, int w, int dims) : k(k), w(w), dims(dims){
	int i;
	h = new HashFunctionH[k];
	for(i = 0; i < k; i++){
		h[i] = HashFunctionH(w, dims);
	}
	for(i = 0; i < k; i++){
		h[i].printVec();
	}
	for(i = 0; i < k; i++){
		this->r.push_back(getUniformRandom());
	}
}

HashFunctionG::~HashFunctionG(){
	delete[] h;
}

int HashFunctionG::computeG(std::vector<double>& p){
	long term;
	int total = 0;
	const unsigned int M = 0xffffffff - 4;
	std::cout << M << std::endl;
	for(int i = 0; i < this->k; i++){
		term = r[i]*h[i].computeH(p);
		std::cout << "1: " << term << std::endl;
		std::cout << "2: " << modulo(term,M) << std::endl;
		total = modulo(total+term,M); //fix for corret mod opperation;
	}
	return modulo(total,M); //%tableSize for G, without for ID
}

int HashFunctionG::modulo(long a,unsigned int b){
	return a%b;//(long)(a%b+b)%b;
}