#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "hashFunctionG.hpp"
#include "randGen.hpp"
#include "hashTable.hpp"

int main() {
//	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//	setRandomSeed(seed);
//	std::vector<double> p = {1, 1, 1};
//	HashFunctionG* g1 = new HashFunctionG(4, 2, 3);
//	std::cout << g1->computeG(p);
//
//	delete g1;

//hash table demo
    Hashtable *ht = new Hashtable(5);
    ht->insert(1, 1, NULL);
    ht->insert(1, 2, NULL);
    ht->insert(4, 64, NULL);
    ht->insert(3, 48, NULL);
    ht->remove(1, 1);
    ht->print();
    delete (ht);

    return 0;
}
