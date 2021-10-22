#include <chrono>
#include "../inc/hashFunctionG.hpp"
#include "../inc/hashTable.hpp"
#include "../inc/point.hpp"
#include "../inc/LSH.hpp"

struct Neighbor {
    Point *point;
    double distance;
};

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

int LSH::addPoint(Point &p) {

    //make sure the point is valid
    if (p.getDimension() != this->dims) {
        return -1;
    }

    //append p in list of points
    points.push_back(p);

    //add it's address in each hashtable
    for (int i = 0; i < L; i++) {
        unsigned int ID = gFunctions[i].computeID(p.getVector());
        unsigned int bucketID = ID % buckets;
        if (hashTables[i].insert(bucketID, ID, &p) != 0) return -1;
    }

    return 0;
}

bool compare(Neighbor a, Neighbor b) {
    return a.distance < b.distance;
}

bool equal(Neighbor a, Neighbor b) {
    return a.point == b.point;
}

int LSH::findKNN(Point &queryPoint, int numOfNN = 1, double r = -1.0) {

    //make sure the query point is valid
    if (queryPoint.getDimension() != this->dims) {
        return -1;
    }

    //keep list of candidate close neighbors
    std::list <Neighbor> candidates;

    //search each hashtable for candidate neighbors
    for (int i = 0; i < L; i++) {
        unsigned int ID = gFunctions[i].computeID(queryPoint.getVector());
        unsigned int bucketID = ID % buckets;

        std::list < Item * > bucket = this->hashTables->getBucket(bucketID);

        std::list<Item *>::iterator it;
        for (it = bucket.begin(); it != bucket.end(); ++it) {
            if ((*it)->key == ID) {
                Neighbor candidate;
                candidate.point = (*it)->data;
                candidate.distance = queryPoint.l2Distance((*it)->data);
                candidates.push_back(candidate);
            }
        }
    }

    std::list<Neighbor>::iterator it;

    unsigned int neighborsFound = 0;

    for (it = candidates.begin(); it != candidates.end(); ++it) {
        if (neighborsFound == numOfNN){
            return 0;
        }

    }

    candidates.unique(equal);   //remove duplicates
    candidates.sort(compare);   //sort by distances

    return 0;
}
