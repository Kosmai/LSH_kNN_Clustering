#include <iostream>
#include <bitset>
#include "../inc/hamming.hpp"


// Returns factorial of n
int factorial(int n){
    int res = 1;
    for (int i = 2; i <= n; i++)
        res = res * i;
    return res;
}

int partFactorial(int max, int min){
    int res = 1;
    for (int i = max; i > min; i--)
        res = res * i;
    return res;
}

int nCr(int n, int r){

    return partFactorial(n, n-r) / factorial(r);
}

void Hamming::fillHammingMasks(int hammingDistance){
    int dist = hammingDistance;
    unsigned int v = 0b0;
    while(dist-- > 0){
        v = (v << 1) | 0b1;
    }
    masks.push_back(v);
    unsigned int w;
    unsigned int t;
    for(int j = 0; j < nCr(k, hammingDistance) - 1; j++){
        t = (v | (v - 1)) + 1;
        w = t | ((((t & -t) / (v & -v)) >> 1) - 1);
        masks.push_back(w);

        v = w;
    }

}

void Hamming::nextDistance(){
    fillHammingMasks(++currentDist);
}

Hamming::Hamming(unsigned int base, int k) : base(base), k(k), currentDist(0){
    masks.push_back(0b0);
}

void Hamming::printMaskList(){
    std::cout << "Mask List:" << std::endl;
    for (auto v : masks)
        std::cout << std::bitset<32>(v) << std::endl;
}

unsigned int Hamming::getNext(){
    unsigned int next;
    if(masks.empty()){
        nextDistance();
    }
    next = masks.front() ^ base;
    masks.pop_front();
    return next;
}