#include <chrono>
#include <numeric>
#include "../inc/hashFunctionF.hpp"
#include "../inc/hashTable.hpp"
#include "../inc/point.hpp"
#include "../inc/hypercube.hpp"
#include "../inc/hamming.hpp"
#include "../inc/LSH.hpp"
#include "../inc/cluster.hpp"

Hypercube::Hypercube() {
    std::cout << "Default Hypercube ctr implicitly called" << std::endl;
}

Hypercube::Hypercube(int dims, int buckets, int L, int k, int w)
        : dims(dims), buckets(buckets), L(L), k(k), w(w) {

    hashTables = new HashTable[L];
    fFunctions = new HashFunctionF[L];
    for (int i = 0; i < L; i++) {
        hashTables[i] = HashTable(buckets);
        fFunctions[i] = HashFunctionF(k, w, dims);
    }
}

Hypercube::Hypercube(const Hypercube &copy)
        : dims(copy.dims), buckets(copy.buckets), L(copy.L), k(copy.k), w(copy.w) {

    std::cout << "Hypercube was copied - Undefined behavior" << std::endl;
    hashTables = copy.hashTables;
    fFunctions = copy.fFunctions;
}

Hypercube::~Hypercube() {

    delete[] hashTables;
    delete[] fFunctions;
    for(auto v : HyperNeighbors){
        delete v;
    }
    for(auto v : realNeighbors){
        delete v;
    }
    for(auto v : points){
        delete v;
    }
}

void Hypercube::printHT(int id) {
    if (id >= L || id < 0) {
        std::cout << "Out of range ID of hashTable" << std::endl;
        return;
    }
    hashTables[id].print();
}

void Hypercube::printAllHT() {
    for (int i = 0; i < L; i++) {
        std::cout << "HashTable " << i + 1 << std::endl;
        hashTables[i].print();
    }
}

int Hypercube::addPoint(Point* p) {
    //make sure the point is valid
    if (p->getDimension() != this->dims) {
        return -1;
    }

    //append p in list of points
    points.push_back(p);

    //add it's address in each hashtable
    for (int i = 0; i < L; i++) {
        unsigned int ID = fFunctions[i].computeID(p->getVector());
        unsigned int bucketID = ID % buckets;
        if (hashTables[i].insert(bucketID, ID, p) != 0) return -1;
    }

    return 0;
}

static bool compare(Neighbor* a, Neighbor* b) {
    return a->distance < b->distance;
}

static bool equal(Neighbor* a, Neighbor* b) {
    return a->point == b->point;
}

void Hypercube::bruteForceSearch(Point &queryPoint){
    std::list<double> distances;
    std::list<Point*>::iterator it;
    for (it = points.begin(); it != points.end(); ++it) {

        Neighbor* candidate = new Neighbor;
        candidate->point = (Point*)(*it);
        candidate->distance = queryPoint.l2Distance((Point*)(*it));

        realNeighbors.push_back(candidate);
        distances.push_back(candidate->distance);
    }
    realNeighbors.sort(compare);

    int avg = std::accumulate(distances.begin(), distances.end(), 0.0) / distances.size();
    std::cout << "Average Distance: " << avg << std::endl;

}


int Hypercube::hyperSearch(Point &queryPoint, int M, int probes) {

    int pointsChecked = 0;
    int probesChecked = 0;
    bool searchFinished = false;
    unsigned int currentBucket;

    //make sure the query point is valid
    if (queryPoint.getDimension() != this->dims) {
        return -1;
    }
    //queryPoint.print();

    //L = 1 always for hypercube, code uses for, for easy expansion to multiple cubes
    for (int i = 0; i < L; i++) {
        unsigned int bucketID = fFunctions[i].computeID(queryPoint.getVector());

        Hamming gen = Hamming(bucketID, k);

        while(probesChecked < probes){
            if(searchFinished){
                probesChecked++;
                break;
            }
            currentBucket = gen.getNext() % this->buckets;
            //gen.printMaskList();
            std::list<Item *> bucket = this->hashTables[i].getBucket(currentBucket);
            std::list<Item *>::iterator it;

            //look through all points in this cube's vectice (provided by hamming class)
            for (it = bucket.begin(); it != bucket.end(); ++it) {
                Neighbor* candidate = new Neighbor;
                candidate->point = (*it)->data;
                candidate->distance = queryPoint.l2Distance((*it)->data);
                HyperNeighbors.push_back(candidate);
                if(++pointsChecked >= M){
                    searchFinished = true;
                    break;
                }
            }
            probesChecked++;
        }
    }
    std::cout << "Found " << pointsChecked << " points in " << probesChecked << " vertices" << std::endl;

    HyperNeighbors.sort(compare);   //sort by distances
    HyperNeighbors.unique(equal);   //remove duplicates

    return 0;
}


void Hypercube::displayResults(Point &queryPoint, unsigned int numOfNN, double r){

    //Print query ID
    std::cout << "Query: " << queryPoint.getId() << std::endl;

    //Print K nearest neighbors
    std::list<Neighbor*>::iterator LSHIterator = HyperNeighbors.begin();
    std::list<Neighbor*>::iterator realIterator = realNeighbors.begin();

    for(unsigned int i = 0; i < numOfNN; i++){

        if(LSHIterator == HyperNeighbors.end() || realIterator == realNeighbors.end()){
            break;
        }

        std::cout << "Nearest Neighbor-" << i+1 << ": " << (*LSHIterator)->point->getId() << std::endl;
        std::cout << "distanceHypercube: " << (*LSHIterator)->distance << std::endl;
        std::cout << "distanceTrue: " << (*realIterator)->distance << std::endl;
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

        LSHIterator++;
        realIterator++;
    }

}

int Hypercube::calculateNN(Point &queryPoint, int M, int probes, unsigned int numOfNN = 1, double r = -1.0){

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto LSH_t1 = high_resolution_clock::now();
    hyperSearch(queryPoint, M, probes);
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

    for (it = HyperNeighbors.begin(); it != HyperNeighbors.end(); ++it) {

        if((*it)->distance > r){
            break;
        }

        std::cout << (*it)->point->getId() << std::endl;
    }

    return 0;
}

void Hypercube::getNearestByR(double r, int rangeIndex, Cluster* clusters, int currentCluster, int probes, int M){

    Point centroid = clusters[currentCluster].getCentroid();

    hyperSearch(centroid, M, probes); //TODO play with parameters

    std::list<Neighbor*>::iterator it;

    for (it = HyperNeighbors.begin(); it != HyperNeighbors.end(); ++it) {

        if((*it)->distance < r){
            Point* point = (*it)->point;

            int pointRangeIndex = point->getRangeIndex();
            int pointCluster    = point->getClusterIndex();

            //if unassigned cluster for this point
            if(pointCluster == -1){
                point->setRangeIndex(rangeIndex);
                point->setClusterIndex(currentCluster);
                //clusters[currentCluster].getClusteredPoints().push_back(point);
            }
            else{
                //if cluster was assigned at this iteration of the algorithm
                if(pointRangeIndex == rangeIndex){
                    //if its closer to this cluster, assign the point to it
                    if(point->l2Distance(&centroid) < point->l2Distance(&clusters[pointCluster].getCentroid())){
                        point->setClusterIndex(currentCluster);
                        //clusters[currentCluster].getClusteredPoints().push_back(point);
                    }
                }
            }
        }
    }
}