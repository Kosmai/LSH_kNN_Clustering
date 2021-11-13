#include <iostream>
#include <bitset>

#include "../inc/hamming.hpp"


//Returns factorial of n
static int factorial(int n){
    int res = 1;
    for (int i = 2; i <= n; i++)
        res = res * i;
    return res;
}

//Returns max * (max - 1) * ... * (min + 1)
static int partFactorial(int max, int min){
    int res = 1;
    for (int i = max; i > min; i--)
        res = res * i;
    return res;
}

//returns n choose r
static int nCr(int n, int r){
    return partFactorial(n, n-r) / factorial(r);
}

void Hamming::fillHammingMasks(int hammingDistance){

    //preforming bit operations to generate all hamming masks
    //that will later be used to get new hamming permutations
    //of the base number.

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

int Hamming::nextDistance(){
    if(currentDist+1 > k){
        return -1;
    }
    fillHammingMasks(++currentDist);
    return 0;
}

Hamming::Hamming(unsigned int base, int k) : base(base), k(k), currentDist(0){
    //base number will be permuted using hamming
    masks.push_back(0b0);
}

void Hamming::printMaskList(){
    std::cout << "Mask List:" << std::endl;
    for (auto v : masks)
        std::cout << std::bitset<32>(v) << std::endl;
}

int Hamming::getNext(){
    int next;

    //if there are no more masks, generate more
    //at a bigger distance
    if(masks.empty()){
        if(nextDistance() < 0){
            return -1;
        }
    }

    //XOR the base number with a mask, then delete it
    next = (int) (masks.front() ^ base);
    masks.pop_front();
    return next;
}