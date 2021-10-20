#include <chrono>
#include "../inc/hashFunctionG.hpp"
#include "../inc/hashTable.hpp"
#include "../inc/point.hpp"
#include "../inc/LSH.hpp"

LSH::LSH(){
	std::cout << "Default LSH ctr implicitly called" << std::endl;
}

LSH::LSH(int dims, int buckets, int L, int k, int w) 
: dims(dims),buckets(buckets), L(L), k(k), w(w){

	hashTables = new HashTable[L];
	gFunctions = new HashFunctionG[L];
	for(int i = 0; i < L; i++){
		hashTables[i] = HashTable(buckets);
		gFunctions[i] = HashFunctionG(k, w, dims);
	}
}

LSH::LSH(const LSH& copy) 
: dims(copy.dims),buckets(copy.buckets), L(copy.L), k(copy.k), w(copy.w){

	std::cout << "LSH was copied - Undefined behavior" << std::endl;
	hashTables = copy.hashTables;
	gFunctions = copy.gFunctions;
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

	//make sure the point is valid
	if(p.getDimension() != this->dims){
		return -1;
	}

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