#pragma once

#include <vector>
#include <map>

/* This module provides the h function class used in LSH / Hypercube */

class HashFunctionH{
private:
    int w;
    int t;
    std::vector<double> v;
    std::map<int, int> bucketBit; //keeps track of already assigned bits

public:
    //constructors-destructors
    HashFunctionH() = default;
    explicit HashFunctionH(int w, int dims);
    HashFunctionH(const HashFunctionH& copy) = default;
    HashFunctionH& operator=(const HashFunctionH &copy) = default;
    ~HashFunctionH() = default;

    //returns h(p) given vector p for LSH
    int computeH(std::vector<double>& p);

    //returns h(p) given vector p for Hypercube
    int computeHyperH(std::vector<double>& p);

    //prints the vector that defines this h function
    void printVec();
};