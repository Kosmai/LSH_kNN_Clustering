#include <iostream>
#include <chrono>
#include <cmath>

#include "../inc/hashFunctionG.hpp"
#include "../inc/hashTable.hpp"
#include "../inc/point.hpp"
#include "../inc/LSH.hpp"
#include "../inc/cluster.hpp"
#include "../inc/timeSeries.hpp"

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
}

std::list<Point*>& LSH::getPoints(){
    return this->points;
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
        unsigned int ID = gFunctions[i].computeG(p->getVector());
        unsigned int bucketID = ID % buckets;
        if (hashTables[i].insert(bucketID, ID, p) != 0) return -1;
    }

    return 0;
}

int LSH::addTimeSeries(TimeSeries* t, double dx, double dy, double filter_e){
   Point* p;
   if(filter_e < 0){
       p = t->snapToGrid(dx, dy);
   }

   else{
       p = t->filter(filter_e);
   }

   addPoint(p);
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

void LSH::bruteForceSearch(Point &queryPoint, int metric = 0){

    std::list<Point*>::iterator it;
    for (it = points.begin(); it != points.end(); ++it) {

        Neighbor* candidate = new Neighbor;
        candidate->point = (Point*)(*it);
        if(metric == 0){
            candidate->distance = queryPoint.l2Distance((Point*)(*it));
        }
        else if(metric == 1){
            candidate->distance = queryPoint.getTimeSeries()->discreteFrechetDistance(candidate->point->getTimeSeries());
        }
        else if(metric == 2){
            candidate->distance = queryPoint.getTimeSeries()->continuousFrechetDistance(candidate->point->getTimeSeries());
        }

        realNeighbors.push_back(candidate);
    }
    realNeighbors.sort(compare);
}

int LSH::LSHSearch(Point &queryPoint, int metric = 0) {
    
    //make sure the query point is valid
    if (queryPoint.getDimension() != this->dims) {
        return -1;
    }

    //clear neighbors from previous queries (if any)
    for(auto v : LSHNeighbors){
        delete v;
    }
    for(auto v : realNeighbors){
        delete v;
    }
    this->LSHNeighbors.clear();
    this->realNeighbors.clear();

    //search each hashtable for candidate neighbors
    for (int i = 0; i < L; i++) {
        unsigned int ID = gFunctions[i].computeG(queryPoint.getVector());
        unsigned int bucketID = ID % buckets;

        std::list <Item *> bucket = this->hashTables[i].getBucket(bucketID);
        std::list<Item *>::iterator it;

        //check appropriate bucket
        for (it = bucket.begin(); it != bucket.end(); ++it) {
            //make sure items have the same ID as the algorithm requires
            if ((*it)->key == ID) {
                Neighbor* candidate = new Neighbor;
                candidate->point = (*it)->data;
                if(metric == 0){
                    candidate->distance = queryPoint.l2Distance((*it)->data);
                }
                else if(metric == 1){
                    candidate->distance = queryPoint.getTimeSeries()->discreteFrechetDistance((*it)->data->getTimeSeries());
                }
                else if(metric == 2){
                    candidate->distance = queryPoint.getTimeSeries()->continuousFrechetDistance((*it)->data->getTimeSeries());
                }

                LSHNeighbors.push_back(candidate);
            }
        }
    }

    LSHNeighbors.sort(compare);   //sort by distances
    LSHNeighbors.unique(equal);   //remove duplicates

    return 0;
}


void LSH::displayResults(Point &queryPoint, FILE* fp, unsigned int numOfNN){

    double avgRatio = 0;
    double dist;
    unsigned int i;

    //Print query ID
    fprintf(fp, "Query: %s\n", queryPoint.getId().c_str());

    //Print K nearest neighbors
    std::list<Neighbor*>::iterator LSHIterator = LSHNeighbors.begin();
    std::list<Neighbor*>::iterator realIterator = realNeighbors.begin();

    for(i = 0; i < numOfNN; i++){

        if(LSHIterator != LSHNeighbors.end() && realIterator == realNeighbors.end()){
            fprintf(fp, "Nearest Neighbor-%d: %s\n", i+1, (*LSHIterator)->point->getId().c_str());
            fprintf(fp, "distanceLSH: %lf\n", (double)(*LSHIterator)->distance);
            LSHIterator++;
            continue;
        }

        //while there are more points
        if(LSHIterator == LSHNeighbors.end() || realIterator == realNeighbors.end()){
            break;
        }

        fprintf(fp, "Nearest Neighbor-%d: %s\n", i+1, (*LSHIterator)->point->getId().c_str());
        fprintf(fp, "distanceLSH: %lf\n", (double)(*LSHIterator)->distance);
        fprintf(fp, "distanceTrue: %lf\n", (double)(*realIterator)->distance);

        //used for debugging purposes
        dist = (double)(*LSHIterator)->distance / (double)(*realIterator)->distance;
        if(dist != 0){
            if(dist > worstDistance){
                worstDistance = dist;
            }
        }
        avgRatio += dist;
        
        //increment counters
        LSHIterator++;
        realIterator++;
    }

    if(i != 0){
        successfulQueries++;
        averageRatio += avgRatio / i;
    }
}

int LSH::calculateNN(Point &queryPoint, FILE* fp, unsigned int numOfNN = 1, double r = -1.0, int metric = 0, bool disableBruteForce = false){

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::microseconds;

    auto LSH_t1 = high_resolution_clock::now();
    LSHSearch(queryPoint, metric);
    auto LSH_t2 = high_resolution_clock::now();

    auto brute_t1 = high_resolution_clock::now();
    if(!disableBruteForce){
        bruteForceSearch(queryPoint, metric);
    }
    auto brute_t2 = high_resolution_clock::now();

    auto LSH_us = duration_cast<microseconds>(LSH_t2 - LSH_t1);
    auto brute_us = duration_cast<microseconds>(brute_t2 - brute_t1);

    displayResults(queryPoint, fp, numOfNN);

    fprintf(fp, "tLSH : %.6lf\n", (double)LSH_us.count()/1000000);
    fprintf(fp, "tTrue: %.6lf\n", (double)brute_us.count()/1000000);

    printf("tLSH : %.6lf\n", (double)LSH_us.count()/1000000);
    printf("tTrue: %.6lf\n", (double)brute_us.count()/1000000);
    printf("---------------\n");

 
    fprintf(fp, "R-near neighbors:\n");

    std::list<Neighbor*>::iterator it;

    for (it = LSHNeighbors.begin(); it != LSHNeighbors.end(); ++it) {

        if((*it)->distance > r){
            break;
        }

        fprintf(fp, "%s\n",(*it)->point->getId().c_str());
    }
    
    return 0;
}

void LSH::getNearestByR(double r, Cluster* clusters, int currentCluster, int metric){

    Point centroid = *(clusters[currentCluster].getCentroid().getTimeSeries()->snapToGrid(1,1));

    LSHSearch(centroid, metric);

    std::list<Neighbor*>::iterator it;

    for (it = LSHNeighbors.begin(); it != LSHNeighbors.end(); ++it) {

        if((*it)->distance < r){
            Point* point = (*it)->point;

            int pointCluster    = point->getClusterIndex();

            //if no cluster has been assigned
            if(pointCluster == -1){
                point->setClusterIndex(currentCluster);
            }
            //if another cluster has been assigned, compare distances
            else if(pointCluster != currentCluster){
                if(metric == 0){
                    if(point->l2Distance(&centroid) < point->l2Distance(&clusters[pointCluster].getCentroid())){
                        point->setClusterIndex(currentCluster);
                    }
                }
                else if(metric == 1){
                    if(point->getTimeSeries()->discreteFrechetDistance(centroid.getTimeSeries()) < point->getTimeSeries()->discreteFrechetDistance(clusters[pointCluster].getCentroid().getTimeSeries())){
                        point->setClusterIndex(currentCluster);
                    }
                }
            }
        }
    }
}

double LSH::calculateW(std::vector<Point*> &points){
    int id;
	int size = points.size();
	double totalDist = 0;
	int i, j;
    
    //uses only a fraction of the points
    int iterSize = sqrt(size);
    //if points are < sqrt(size), just use them all
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

void LSH::resetStatistics(){
   	averageRatio = 0;
	worstDistance = DBL_MIN;
    successfulQueries = 0;
}