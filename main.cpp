#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "hashFunctionG.hpp"
#include "randGen.hpp"
#include "readInput.hpp"
#include "hashTable.hpp"
#define MAX_ELEMENTS 10000
#define BUCKETS 1
#define DIMS 2


int main() {
//	Hashtable *ht = new Hashtable(BUCKETS);
//
//	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//	setRandomSeed(seed);
//	std::vector<double> p[MAX_ELEMENTS];
//	for(int i = 0; i < MAX_ELEMENTS; i++){
//		for(int j = 0; j < DIMS; j++){
//			p[i].push_back(getNormalRandom()*MAX_ELEMENTS);
//		}
//	}
//
//	unsigned int ID;
//	HashFunctionG* g1 = new HashFunctionG(4, 2, DIMS);
//	for(int i = 0; i < MAX_ELEMENTS; i++){
//		ID = g1->computeID(p[i]);
//		ht->insert(ID%BUCKETS, ID, NULL);
//	}
//	int c;
//	for(int i = 0; i < MAX_ELEMENTS; i++){
//		ID = g1->computeID(p[i]);
//		if((c = ht->count(ID%BUCKETS, ID)) != 1){
//			std::cout << c << std::endl;
//		}
//	}
//
//	//ht->print();
//	delete (g1);
//	delete (ht);

//read input demo
    readDataSet("datasets/dataset1", '\t');
	return 0;
}
