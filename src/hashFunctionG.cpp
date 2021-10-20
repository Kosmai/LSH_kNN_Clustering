#include <iostream>
#include "../inc/hashFunctionG.hpp"
#include "../inc/hashFunctionH.hpp"
#include "../inc/randGen.hpp"

HashFunctionG::HashFunctionG(){
	std::cout << "Dummy G created" << std::endl;
};

//Initialize k h functions and k "r" values.
HashFunctionG::HashFunctionG(int k, int w, int dims) : k(k), w(w), dims(dims){
	h = new HashFunctionH[k];
	for(int i = 0; i < k; i++){
		h[i] = HashFunctionH(w, dims);
	}
	// for(i = 0; i < k; i++){
	// 	h[i].printVec();
	// }
	for(int i = 0; i < k; i++){
		this->r.push_back(getUniformRandom());
	}
	std::cout << "Normal G created" << std::endl;
}

HashFunctionG::~HashFunctionG(){
	std::cout << "G deleted" << std::endl;
	delete[] h;
}

HashFunctionG& HashFunctionG::operator=(const HashFunctionG &copy){
    std::cout << "HashFunctionG Copy Opp Created" << std::endl;
    this->k = copy.k;
    this->w = copy.w;
    this->dims = copy.dims;
    h = new HashFunctionH[k];
	for(int i = 0; i < k; i++){
		h[i] = HashFunctionH(w, dims);
	}
	for(int i = 0; i < k; i++){
		this->r.push_back(getUniformRandom());
	}
    return *this;
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