#include <chrono>
#include "hashFunctionG.hpp"
#include "hashTable.hpp"
#include "randGen.hpp"
#include "point.hpp"
#include "LSH.hpp"


LSH::LSH(int dims, int buckets, int L, int k, int w) 
: dims(dims),buckets(buckets), L(L), k(k), w(w){
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	setRandomSeed(seed);

	hashTables = new HashTable[L];
	gFunctions = new HashFunctionG[L];
	for(int i = 0; i < L; i++){
		hashTables[i] = HashTable(buckets);
		gFunctions[i]  = HashFunctionG(k, w, dims);
	}
}

LSH::~LSH(){
	delete[] hashTables;
	delete[] gFunctions;
}

void LSH::printHT(int id){
	if(id >= L || id < 0){
		std::cout << "Out of range ID of hashTable" << std::endl;
		return;
	}
	hashTables[id].print();
}

void LSH::printAllHT(){
	for(int i = 0; i < L; i++){
		std::cout << "HashTable " << i+1 << std::endl;
		hashTables[i].print();
	}
}

int LSH::addPoint(Point& p){

	//append p in list of points
	points.push_back(p);

	//add it's address in each hashtable 
	for(int i = 0; i < L; i++){
		unsigned int ID = gFunctions[i].computeID(p.getVector());
		unsigned int bucketID = ID % buckets;
		if(hashTables[i].insert(bucketID, ID, &p) != 0) return -1;
	}

	return 0;
}