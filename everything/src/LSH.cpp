#include <chrono>
#include "../inc/hashFunctionG.hpp"
#include "../inc/hashTable.hpp"
#include "../inc/point.hpp"
#include "../inc/LSH.hpp"

LSH::LSH() {
    std::cout << "Default LSH ctr implicitly called" << std::endl;
}

LSH::LSH(int dims, int buckets, int L, int k, int w)
        : dims(dims), buckets(buckets), L(L), k(k), w(w) {

    hashTables = new HashTable[L];
    gFunctions = new HashFunctionG[L];
    for (int i = 0; i < L; i++) {
        hashTables[i] = HashTable(buckets);
        gFunctions[i] = HashFunctionG(k, w, dims);
    }
}

LSH::LSH(const LSH &copy)
        : dims(copy.dims), buckets(copy.buckets), L(copy.L), k(copy.k), w(copy.w) {

    std::cout << "LSH was copied - Undefined behavior" << std::endl;
    hashTables = copy.hashTables;
    gFunctions = copy.gFunctions;
}

LSH::~LSH() {

    delete[] hashTables;
    delete[] gFunctions;
    for(auto v : LSHNeighbors){
        delete v;
    }
    for(auto v : realNeighbors){
        delete v;
    }
    for(auto v : points){
        delete v;
    }
}

void LSH::printHT(int id) {
    if (id >= L || id < 0) {
        std::cout << "Out of range ID of hashTable" << std::endl;
        return;
    }
    hashTables[id].print();
}

void LSH::printAllHT() {
    for (int i = 0; i < L; i++) {
        std::cout << "HashTable " << i + 1 << std::endl;
        hashTables[i].print();
    }
}

int LSH::addPoint(Point* p) {

    //make sure the point is valid
    if (p->getDimension() != this->dims) {
        return -1;
    }

    //append p in list of points
    points.push_back(p);

    //add it's address in each hashtable
    for (int i = 0; i < L; i++) {
        unsigned int ID = gFunctions[i].computeID(p->getVector());
        unsigned int bucketID = ID % buckets;
        if (hashTables[i].insert(bucketID, ID, p) != 0) return -1;
    }

    return 0;
}

static bool compare(Neighbor* a, Neighbor* b) {
    return a->distance < b->distance;
}

static bool equal(Neighbor* a, Neighbor* b) {
    bool equality = (a->point == b->point);
    if(equality){delete b;}
    return equality;
}

void LSH::bruteForceSearch(Point &queryPoint){
    std::list<Point*>::iterator it;
    for (it = points.begin(); it != points.end(); ++it) {

        Neighbor* candidate = new Neighbor;
        candidate->point = (Point*)(*it);
        candidate->distance = queryPoint.l2Distance((Point*)(*it));

        realNeighbors.push_back(candidate);
    }
    realNeighbors.sort(compare);

}

int LSH::LSHSearch(Point &queryPoint) {

    //make sure the query point is valid
    if (queryPoint.getDimension() != this->dims) {
        return -1;
    }
    //queryPoint.print();

    //search each hashtable for candidate neighbors
    for (int i = 0; i < L; i++) {
        unsigned int ID = gFunctions[i].computeID(queryPoint.getVector());
        unsigned int bucketID = ID % buckets;

        std::list <Item *> bucket = this->hashTables[i].getBucket(bucketID);

        std::list<Item *>::iterator it;
        //std::cout << "L " << i << std::endl;
        for (it = bucket.begin(); it != bucket.end(); ++it) {

            if ((*it)->key == ID) {
                Neighbor* candidate = new Neighbor;
                candidate->point = (*it)->data;
                candidate->distance = queryPoint.l2Distance((*it)->data);

                LSHNeighbors.push_back(candidate);
                //if(queryPoint.l2Distance((*it)->data) < 350){
                //std::cout << "\tDis "<< queryPoint.l2Distance((*it)->data) << std::endl;}
            }
        }
    }

    LSHNeighbors.sort(compare);   //sort by distances
    LSHNeighbors.unique(equal);   //remove duplicates



    return 0;
}


void LSH::displayResults(Point &queryPoint, unsigned int numOfNN, double r){

    //Print query ID
    std::cout << "Query: " << queryPoint.getId() << std::endl;

    //Print K nearest neighbors
    std::list<Neighbor*>::iterator LSHIterator = LSHNeighbors.begin();
    std::list<Neighbor*>::iterator realIterator = realNeighbors.begin();

    for(unsigned int i = 0; i < numOfNN; i++){

        if(LSHIterator == LSHNeighbors.end() || realIterator == realNeighbors.end()){
            break;
        }

        std::cout << "Nearest Neighbor-" << i+1 << ": " << (*LSHIterator)->point->getId() << std::endl;
        std::cout << "distanceLSH: " << (*LSHIterator)->distance << std::endl;
        std::cout << "distanceTrue: " << (*realIterator)->distance << std::endl;
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

        LSHIterator++;
        realIterator++;
    }

}

int LSH::calculateNN(Point &queryPoint, unsigned int numOfNN = 1, double r = -1.0){

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto LSH_t1 = high_resolution_clock::now();
    LSHSearch(queryPoint);
    auto LSH_t2 = high_resolution_clock::now();

    auto brute_t1 = high_resolution_clock::now();
    bruteForceSearch(queryPoint);
    auto brute_t2 = high_resolution_clock::now();

    auto LSH_ms = duration_cast<milliseconds>(LSH_t2 - LSH_t1);
    auto brute_ms = duration_cast<milliseconds>(brute_t2 - brute_t1);

    displayResults(queryPoint, numOfNN, r);

    std::cout << "tLSH: "  << LSH_ms.count() << "ms" << std::endl;
    std::cout << "tTrue: " << brute_ms.count() << "ms" << std::endl;
 
    std::cout << "R-near neighbors:"<< std::endl;

    std::list<Neighbor*>::iterator it;

    for (it = LSHNeighbors.begin(); it != LSHNeighbors.end(); ++it) {

        if((*it)->distance > r){
            break;
        }

        std::cout << (*it)->point->getId() << std::endl;
    }
    
    return 0;
}