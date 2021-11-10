#include <iostream>
#include <chrono>

#include "../inc/hashFunctionG.hpp"
#include "../inc/hashTable.hpp"
#include "../inc/point.hpp"
#include "../inc/LSH.hpp"
#include "../inc/cluster.hpp"

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
                candidate->distance = queryPoint.l2Distance((*it)->data);

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

        //while there are more points
        if(LSHIterator == LSHNeighbors.end() || realIterator == realNeighbors.end()){
            i++;
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

    averageRatio += avgRatio / i;

}

int LSH::calculateNN(Point &queryPoint, FILE* fp, unsigned int numOfNN = 1, double r = -1.0){

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

    displayResults(queryPoint, fp, numOfNN);

    fprintf(fp, "tLSH:  %.3lf\n", (double)LSH_ms.count()/1000);
    fprintf(fp, "tTrue: %.3lf\n", (double)brute_ms.count()/1000);

    std::cout << "tLSH: "  << (double)LSH_ms.count()/1000 << std::endl;
    std::cout << "tTrue: " << (double)brute_ms.count()/1000 << std::endl;

 
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

//TODO make sure it works fine and add comments
void LSH::getNearestByR(double r, int rangeIndex, Cluster* clusters, int currentCluster){

    Point centroid = clusters[currentCluster].getCentroid();

    LSHSearch(centroid);

    std::list<Neighbor*>::iterator it;

    for (it = LSHNeighbors.begin(); it != LSHNeighbors.end(); ++it) {

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
                else{
                    //if cluster was assigned at any previous iteration, steal it
                    point->setRangeIndex(rangeIndex);
                    point->setClusterIndex(currentCluster);
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
