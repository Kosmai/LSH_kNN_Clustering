#include <iostream>
#include "hashFunctionG.hpp"
#include "hashFunctionH.hpp"
#include "randGen.hpp"

HashFunctionG::HashFunctionG(){};

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

unsigned int HashFunctionG::computeID(std::vector<double>& p){
	long term;
	unsigned int termMod;
	unsigned int total = 0;
	const unsigned int M = 0xffffffff - 4; //prime 2^32 - 5
	for(int i = 0; i < this->k; i++){
		term = (long)r[i]*h[i].computeH(p);
		termMod = modulo(term,M);
		total = modulo((long)total+termMod,M);
	}
	return total;
}

unsigned int HashFunctionG::computeG(std::vector<double>& p, unsigned int tableSize){
	return this->computeID(p) % tableSize;
}

unsigned int HashFunctionG::modulo(long a,unsigned int b){
	return (long)(a%b+b)%b;
}