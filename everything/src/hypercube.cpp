#include <iostream>
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

std::list<Point*>& Hypercube::getPoints(){
    return this->points;
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
        unsigned int ID = fFunctions[i].computeF(p->getVector());
        unsigned int bucketID = ID % buckets;
        if (hashTables[i].insert(bucketID, ID, p) != 0) return -1;
    }

    return 0;
}

//Used when sorting the lists
static bool compare(Neighbor* a, Neighbor* b) {
    return a->distance < b->distance;
}

//Used when removing duplicates of items in lists
static bool equal(Neighbor* a, Neighbor* b) {
    bool equality = (a->point == b->point);
    if(equality){delete b;}
    return equality;
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

    double avg = std::accumulate(distances.begin(), distances.end(), 0.0) / distances.size();
    std::cout << "Average Distance: " << avg << std::endl;

}


int Hypercube::hyperSearch(Point &queryPoint, int M, int probes) {

    std::cout << M << ", probes: " << probes << std::endl;

    int pointsChecked = 0;
    int probesChecked = 0;
    bool searchFinished = false;
    int currentBucket;

    //make sure the query point is valid
    if (queryPoint.getDimension() != this->dims) {
        return -1;
    }

    //clear neighbors from previous queries (if any)
    for(auto v : HyperNeighbors){
        delete v;
    }
    for(auto v : realNeighbors){
        delete v;
    }
    this->HyperNeighbors.clear();
    this->realNeighbors.clear();

    //L = 1 always for hypercube, code uses for, for easy expansion to multiple cubes
    //search each hashtable for candidate neighbors
    for (int i = 0; i < L; i++) {
        unsigned int bucketID = fFunctions[i].computeF(queryPoint.getVector());

        Hamming gen = Hamming(bucketID, k);

        while(probesChecked < probes){
            if(searchFinished){
                break;
            }
            currentBucket = gen.getNext() % this->buckets;
            if(currentBucket < 0){
                break;
            }

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

void Hypercube::displayResults(Point &queryPoint, FILE* fp, unsigned int numOfNN, double r){

    //Print query ID
    fprintf(fp, "Query: %s\n", queryPoint.getId().c_str());

    //Print K nearest neighbors
    std::list<Neighbor*>::iterator HyperIterator = HyperNeighbors.begin();
    std::list<Neighbor*>::iterator realIterator = realNeighbors.begin();
    
    unsigned int i;
    double avgRatio = 0;
    double dist;

    for(i = 0; i < numOfNN; i++){

        //while there are more points
        if(HyperIterator == HyperNeighbors.end() || realIterator == realNeighbors.end()){
            break;
        }

        fprintf(fp, "Nearest Neighbor-%d: %s\n", i+1, (*HyperIterator)->point->getId().c_str());
        fprintf(fp, "distanceHypercube: %lf\n", (double)(*HyperIterator)->distance);
        fprintf(fp, "distanceTrue: %lf\n", (double)(*realIterator)->distance);

        //used for debugging purposes
        dist = (double)(*HyperIterator)->distance / (double)(*realIterator)->distance;
        if(dist != 0){
            if(dist > worstDistance){
                worstDistance = dist;
            }
        }
        avgRatio += dist;

        //increment counters
        HyperIterator++;
        realIterator++;
    }

    if(i != 0){
        successfulQueries++;
        averageRatio += avgRatio / i;
    }
}


int Hypercube::calculateNN(Point &queryPoint, FILE* fp, int M, int probes, unsigned int numOfNN = 1, double r = -1.0){

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto Hyper_t1 = high_resolution_clock::now();
    hyperSearch(queryPoint, M, probes);
    auto Hyper_t2 = high_resolution_clock::now();

    auto brute_t1 = high_resolution_clock::now();
    bruteForceSearch(queryPoint);
    auto brute_t2 = high_resolution_clock::now();

    auto Hyper_ms = duration_cast<milliseconds>(Hyper_t2 - Hyper_t1);
    auto brute_ms = duration_cast<milliseconds>(brute_t2 - brute_t1);

    displayResults(queryPoint, fp, numOfNN, r);

    fprintf(fp, "tHypercube:  %.3lf\n", (double)Hyper_ms.count()/1000);
    fprintf(fp, "tTrue: %.3lf\n", (double)brute_ms.count()/1000);

    std::cout << "tHyper: "  << (double)Hyper_ms.count()/1000 << std::endl;
    std::cout << "tTrue: " << (double)brute_ms.count()/1000 << std::endl;

    fprintf(fp, "R-near neighbors:\n");

    std::list<Neighbor*>::iterator it;

    for (it = HyperNeighbors.begin(); it != HyperNeighbors.end(); ++it) {

        if((*it)->distance > r){
            break;
        }

        fprintf(fp, "%s\n",(*it)->point->getId().c_str());

    }

    return 0;
}

void Hypercube::getNearestByR(double r, Cluster* clusters, int currentCluster, int probes, int M){

    Point centroid = clusters[currentCluster].getCentroid();

    hyperSearch(centroid, M, probes);

    std::list<Neighbor*>::iterator it;

    for (it = HyperNeighbors.begin(); it != HyperNeighbors.end(); ++it) {

        if((*it)->distance < r){
            Point* point = (*it)->point;

            int pointCluster    = point->getClusterIndex();

            //if no cluster has been assigned
            if(pointCluster == -1){
                point->setClusterIndex(currentCluster);
            }
            //if another cluster has been assigned, compare distances
            else if(pointCluster != currentCluster){
                if(point->l2Distance(&centroid) < point->l2Distance(&clusters[pointCluster].getCentroid())){
                    point->setClusterIndex(currentCluster);
                }
            }
        }
    }
}

double Hypercube::calculateW(std::vector<Point*> &points){
    int id;
	int size = points.size();
	double totalDist = 0;
	int i, j;
    
    //uses only a fraction of the points
    int iterSize = size/128;
    //if points are < 128, just use them all
    if(iterSize < 1){
        iterSize = size;
    }

    //calculate distances between random points
	for(i = 0; i < iterSize; i++){
		id = rand() % size;
		for(j = 0; j < iterSize; j++){
			int otherId = rand() % size;
			if(otherId == id){
				j--;
				continue;
			}
			totalDist += points[id]->l2Distance(points[otherId]);
		}
	}

    //take the average of the distances found
	double averageDistance = totalDist /= i*j;

	return averageDistance;
}

void Hypercube::resetStatistics(){
   	averageRatio = 0;
	worstDistance = DBL_MIN;
    successfulQueries = 0;
}