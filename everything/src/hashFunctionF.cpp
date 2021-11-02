#include <iostream>
#include "../inc/hashFunctionF.hpp"
#include "../inc/hashFunctionH.hpp"
#include "../inc/randGen.hpp"

//Initialize k h functions and k "r" values.
HashFunctionF::HashFunctionF(int k, int w, int dims) : k(k), w(w), dims(dims){
    h = new HashFunctionH[k];
    for(int i = 0; i < k; i++){
        h[i] = HashFunctionH(w, dims);
        this->r.push_back(getUniformRandom());
    }
}

HashFunctionF::~HashFunctionF(){
    delete[] h;
}

HashFunctionF& HashFunctionF::operator=(const HashFunctionF &copy){
    this->k = copy.k;
    this->w = copy.w;
    this->dims = copy.dims;
    h = new HashFunctionH[k];
    for(int i = 0; i < k; i++){
        h[i] = HashFunctionH(w, dims);
        this->r.push_back(getUniformRandom());
    }
    return *this;
}



unsigned int HashFunctionF::computeID(std::vector<double>& p){
    int term;
    unsigned int total = 0;
    for(int i = 0; i < this->k; i++){
        term = h[i].computeHyperH(p);
        total = total | term;
        if(i == this->k-1){
            break;
        }

        total = total << 1;
    }

    return total;
}

unsigned int HashFunctionF::computeF(std::vector<double>& p, unsigned int& tableSize){
    return this->computeID(p) % tableSize;
}
