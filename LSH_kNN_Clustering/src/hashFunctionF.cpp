#include <iostream>

#include "../inc/hashFunctionF.hpp"
#include "../inc/hashFunctionH.hpp"
#include "../inc/randGen.hpp"

HashFunctionF::HashFunctionF(int k, int w, int dims) : k(k), w(w), dims(dims){
    //Initialize k h functions and k "r" values.
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


unsigned int HashFunctionF::computeF(std::vector<double>& p){
    int term;
    unsigned int total = 0;
    for(int i = 0; i < this->k; i++){
        term = h[i].computeHyperH(p);
        total = total | term;  //concatenate term with total
        if(i == this->k-1){
            //if there are no more h functions
            break;
        }

        total = total << 1;  //shift total 1 bit
    }

    return total;
}

unsigned int HashFunctionF::computeID(std::vector<double>& p, unsigned int& tableSize){
    return this->computeF(p) % tableSize;
}
