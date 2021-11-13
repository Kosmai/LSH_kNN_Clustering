#include <iostream>
#include <numeric>

#include "../inc/hashFunctionH.hpp"
#include "../inc/randGen.hpp"

HashFunctionH::HashFunctionH(int w, int dims) : w(w){
    t = getUniformRandomFloat()*w; //Generate t = random double in [0,w]
    //Fill v with random doubles
    for(int i = 0; i < dims; i++){
        this->v.push_back(getNormalRandom()); //Append normal dist. double to the vector
    }
}

int HashFunctionH::computeH(std::vector<double>& p){
    //Using formula: h(p) = (p dot v + t) / w
    return (std::inner_product(std::begin(this->v), std::end(this->v), std::begin(p), 0.0) + t)/w;
}

int HashFunctionH::computeHyperH(std::vector<double>& p){
    int bucketIndex = computeH(p);
    //check if bit has been previously assigned, else assign the bit
    if(this->bucketBit.find(bucketIndex) != this->bucketBit.end()){
        this->bucketBit[bucketIndex] = tossCoin();
    }
    return this->bucketBit[bucketIndex];
}

void HashFunctionH::printVec(){
    std::cout << "Vector v" << std::endl << "=================" << std::endl;
    for(auto i : this->v){
        std::cout << "|-- > " << i << std::endl;
    }
    std::cout << "=================" << std::endl;
}
