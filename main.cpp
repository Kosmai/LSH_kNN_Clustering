#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "hashFunctionG.hpp"
#include "randGen.hpp"
#include "hashTable.hpp"
#define MAX_ELEMENTS 100
#define BUCKETS 1
#define DIMS 2
#define L 8


int main() {
	Hashtable* ht = new Hashtable[L];
	//std::vector<HashFunctionG*> g;
	for(int i = 0; i < L; i++){
		ht[i] = Hashtable(BUCKETS);
		//g[i]  = HashFunctionG(4, 2, DIMS);
	}

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	setRandomSeed(seed);
	std::vector<double> p[MAX_ELEMENTS];
	for(int i = 0; i < MAX_ELEMENTS; i++){
		for(int j = 0; j < DIMS; j++){
			p[i].push_back(getNormalRandom()*MAX_ELEMENTS);
		}
	}

	// unsigned int ID;
	// for(int i = 0; i < MAX_ELEMENTS; i++){
	// 	ID = g[0].computeID(p[i]);
	// 	ht[0].insert(ID%BUCKETS, ID, NULL);
	// }
	// int c;
	// for(int i = 0; i < MAX_ELEMENTS; i++){
	// 	ID = g[0].computeID(p[i]);
	// 	if((c = ht[0].count(ID%BUCKETS, ID)) != 1){
	// 		std::cout << c << std::endl;
	// 	}
	// }

	//ht->print();
	//delete[] ht;
	return 0;
}
