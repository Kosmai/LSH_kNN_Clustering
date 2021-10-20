#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "hashFunctionG.hpp"
#include "randGen.hpp"
#include "hashTable.hpp"
#include "point.hpp"
#include "LSH.hpp"

#define MAX_ELEMENTS 100
#define BUCKETS 10
#define DIMS 2
#define W 2
#define K 4
#define L 8


int main() {
//	Hashtable* ht = new Hashtable[L];
//	HashFunctionG* g = new HashFunctionG[L];
//	for(int i = 0; i < L; i++){
//		ht[i] = Hashtable(BUCKETS);
//		g[i]  = HashFunctionG(K, W, DIMS);
//	}
//
//	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//	setRandomSeed(seed);
//	std::vector<double> p[MAX_ELEMENTS];
//	for(int i = 0; i < MAX_ELEMENTS; i++){
//		for(int j = 0; j < DIMS; j++){
//			p[i].push_back(getNormalRandom()*10);
//		}
//	}
//
//	unsigned int ID;
//	for(int i = 0; i < MAX_ELEMENTS; i++){
//		ID = g[0].computeID(p[i]);
//		ht[0].insert(ID%BUCKETS, ID, NULL);
//	}
//	int c;
//	for(int i = 0; i < MAX_ELEMENTS; i++){
//		c = 0;
//		ID = g[0].computeID(p[i]);
//		if((c = ht[0].count(ID%BUCKETS, ID)) != 1){
//			std::cout << c << std::endl;
//		}
//	}
//
//	ht[0].print();
//	delete[] ht;

//Point demo
    std::vector<double> tempVec = {1, 2};
    std::string tempId = "point1";

    // std::vector<double> tempVec2 = {4, 6, 3, 4};
    // std::string tempId2 = "point2";

    Point a(tempVec, tempId);
    // Point b(tempVec2, tempId2);
    // a.print();
    // std::cout << "Size: " << a.getDimension() << std::endl;
    // std::cout << "Distance: " << a.l2Distance(&b) << std::endl;
    // std::cout << "Distance: " << a.l2Distance({4, 6, 3, 4}) << std::endl;
    // std::cout << "Distance: " << a.l2Distance(b.getVector()) << std::endl;

    LSH lsh = LSH(DIMS, BUCKETS, L, K, W);
    lsh.addPoint(a);
    lsh.printAllHT();
    return 0;
}
