#include <iostream>
#include <cfloat>
#include <algorithm>
#include <cmath>
#include "../inc/kmeans.hpp"
#include "../inc/randGen.hpp"
#include "../inc/LSH.hpp"
#include "../inc/hypercube.hpp"
#include "../inc/readInput.hpp"
#include "../inc/point.hpp"
#include "../inc/cluster.hpp"

Kmeans::Kmeans(unsigned int dimension, unsigned int numOfClusters) : dimension(dimension),
                                                                     numOfClusters(numOfClusters) {
    clusters = new Cluster[numOfClusters];

    for (unsigned int i = 0; i < numOfClusters; i++) {
        clusters[i] = Cluster(dimension);
    }
}

Kmeans::~Kmeans() {
    delete[] clusters;
}

int Kmeans::addPoint(Point *point) {
    this->points.push_back(point);
    return 0;
}

int Kmeans::initializeCentroids(std::list<Point *> &points, centroidInitializationMethod method){

    if(this->numOfClusters > points.size() || this->numOfClusters <= 0){
        return -1;
    }

    std::vector <Point *> centroids;

    std::cout << "Attempting to find centroids\n";

    if (method == Random) {
        if(this->findRandomCentroids(points, this->numOfClusters, centroids) < 0) return -1;
    } else if (method == PlusPlus) {
        if(this->findPlusPlusCentroids(points, this->numOfClusters, centroids) < 0) return -1;
    } else {
        return -1;
    }
    std::cout << "Found centroids\n";

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        this->clusters[i].setCentroid(centroids[i]);
    }
    return 0;
}

int Kmeans::computeLoyd(double iterThreshold, unsigned int maxIters, centroidInitializationMethod method) {

    if(initializeCentroids(this->points, method) < 0) return -1;

    double dist, minDist;
    unsigned int minIndex;

    unsigned int iter;
    double sumOfCentroidMoves = DBL_MAX;

    for (iter = 0; iter < maxIters; iter++) {

        for (unsigned int i = 0; i < this->numOfClusters; i++) {
            this->clusters[i].clearList();
        }

        for (auto point: points) {
            minDist = DBL_MAX;
            minIndex = -1;
            for (unsigned i = 0; i < this->numOfClusters; i++) {
                dist = clusters[i].getCentroid().l2Distance(point);
                if (dist <= minDist) {
                    minDist = dist;
                    minIndex = i;
                }
            }
            this->clusters[minIndex].insertPoint(point);
            point->setClusterIndex(minIndex);
        }
        if (sumOfCentroidMoves < iterThreshold)break;
        sumOfCentroidMoves = 0;

        for (unsigned int i = 0; i < this->numOfClusters; i++) {
            sumOfCentroidMoves += this->clusters[i].recenter();
        }

        std::cout << sumOfCentroidMoves << std::endl;

    }

    std::cout << "Total Iterations: " << iter << std::endl;

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        std::cout << "Items in cluster " << i << ": " << clusters[i].count() << std::endl;
        //clusters[i].print();
    }

    return 0;
}

int Kmeans::computeLSH(double maxRadius, unsigned int maxIters, centroidInitializationMethod method,
                       int buckets, int L, int k, int w) {

    double minDistance;
    double distance;
    int minIndex;

    LSH* lsh = new LSH(this->dimension, buckets, L, k, w);

    for(auto point: this->points){
        lsh->addPoint(point);
    }

    if(initializeCentroids(this->points, method) < 0) return -1;

    int radius = calculateInitialRadius();
    std::cout << "RAD = " << radius << std::endl;

    unsigned int iter;
    double sumOfCentroidMoves = DBL_MAX;

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        this->clusters[i].clearList();
    }

    for (iter = 0; iter < maxIters; iter++) {

        for(unsigned int i = 0; i < this->numOfClusters; i++){
            //std::cout << "Calculating points for cluster i\n";
            lsh->getNearestByR(radius, this->clusters, i);
        }

        //clear all points in clusters
        for(unsigned int i = 0; i < this->numOfClusters; i++){
            this->clusters[i].clearList();
        }

        //assign points to clusters
        for(auto p : lsh->getPoints()){
            int idx = p->getClusterIndex();
            if(idx != -1){
                this->clusters[idx].insertPoint(p);
            }
            else{

            }
        }

        //if (sumOfCentroidMoves < iterThreshold)break;
        sumOfCentroidMoves = 0;

        for (unsigned int i = 0; i < this->numOfClusters; i++) {
            sumOfCentroidMoves += this->clusters[i].recenter();
            std::cout << sumOfCentroidMoves << std::endl;
        }

        radius *= 2;
        if(radius >= maxRadius)break;
        std::cout << radius << std::endl;

    }
    int unassignedPoints = 0;
    for(auto p : lsh->getPoints()){
        if(p->getClusterIndex() == -1){
            unassignedPoints++;
            minDistance = DBL_MAX;
            minIndex = -1;

            for(unsigned int i = 0; i < this->numOfClusters; i++){
                distance = p->l2Distance(this->clusters[i].getCentroid().getVector());
                if(distance <= minDistance){
                    minDistance = distance;
                    minIndex = i;
                }
            }   
            p->setClusterIndex(minIndex);
            this->clusters[minIndex].insertPoint(p);
        }
    }
    std::cout << "-->" <<  unassignedPoints << std::endl;
    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        sumOfCentroidMoves += this->clusters[i].recenter();
    }


    unsigned int pointsAssigned = 0;

    std::cout << "Total Iterations: " << iter << std::endl;

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        unsigned int clusterPoints = clusters[i].count();
        std::cout << "Items in cluster " << i << ": " << clusterPoints << std::endl;
        pointsAssigned += clusterPoints; 
    }

    std::cout << "Total points assigned: " << pointsAssigned << std::endl;

    delete lsh;
    return 0;
}



int Kmeans::computeHypercube(double maxRadius, unsigned int maxIters, centroidInitializationMethod method,
                             int d, int w, int probes, int M) {

    double minDistance;
    double distance;
    int minIndex;

    Hypercube* hypercube = new Hypercube(this->dimension, (int)pow(2,d), 1, d, w);

    for(auto point: this->points){
        hypercube->addPoint(point);
    }

    if(initializeCentroids(this->points, method) < 0) return -1;

    int radius = calculateInitialRadius();

    unsigned int iter;
    double sumOfCentroidMoves = DBL_MAX;

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        this->clusters[i].clearList();
    }

    for (iter = 0; iter < maxIters; iter++) {

        for(unsigned int i = 0; i < this->numOfClusters; i++){
            //std::cout << "Calculating points for cluster i\n";
            hypercube->getNearestByR(radius, this->clusters, i, probes, M);
        }

        //clear all points in clusters
        for(unsigned int i = 0; i < this->numOfClusters; i++){
            this->clusters[i].clearList();
        }

        //assign points to clusters
        for(auto p : this->points){
            int idx = p->getClusterIndex();
            if(idx != -1){
                this->clusters[idx].insertPoint(p);
            }
            else{

            }
        }

        //if (sumOfCentroidMoves < iterThreshold)break;
        sumOfCentroidMoves = 0;

        for (unsigned int i = 0; i < this->numOfClusters; i++) {
            sumOfCentroidMoves += this->clusters[i].recenter();
        }

        radius *= 2;
        if(radius >= maxRadius)break;
        std::cout << radius << std::endl;

    }

    int unassignedPoints = 0;
    for(auto p : hypercube->getPoints()){
        if(p->getClusterIndex() == -1){
            unassignedPoints++;
            minDistance = DBL_MAX;
            minIndex = -1;

            for(unsigned int i = 0; i < this->numOfClusters; i++){
                distance = p->l2Distance(this->clusters[i].getCentroid().getVector());
                if(distance <= minDistance){
                    minDistance = distance;
                    minIndex = i;
                }
            }   
            p->setClusterIndex(minIndex);
            this->clusters[minIndex].insertPoint(p);
        }
    }
    std::cout << "-->" <<  unassignedPoints << std::endl;
    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        sumOfCentroidMoves += this->clusters[i].recenter();
    }

    unsigned int pointsAssigned = 0;

    std::cout << "Total Iterations: " << iter << std::endl;

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        unsigned int clusterPoints = clusters[i].count();
        std::cout << "Items in cluster " << i << ": " << clusterPoints << std::endl;
        pointsAssigned += clusterPoints;
    }

    std::cout << "Total points assigned: " << pointsAssigned << std::endl;

    delete hypercube;
    return 0;
}

int Kmeans::findRandomCentroids(std::list<Point *> &points, unsigned int k, std::vector<Point *> &centroids) {
    if(points.size() <= 0){
        return -1;
    }

    std::vector < Point * > shuffledPoints(points.begin(), points.end());
    std::random_shuffle(std::begin(shuffledPoints), std::end(shuffledPoints));
    for (unsigned int i = 0; i < k; i++) {
        centroids.push_back(shuffledPoints[i]);
    }
    return 0;
}

int Kmeans::findPlusPlusCentroids(std::list<Point *> &points, unsigned int k, std::vector<Point *> &centroids) {
    int size = points.size();
    if(size <= 0){
        return -1;
    }

    std::vector <Point *> shuffledPoints(points.begin(), points.end());
    std::random_shuffle(std::begin(shuffledPoints), std::end(shuffledPoints));
    centroids.push_back(shuffledPoints[0]); //assign 1 centroid at random
    shuffledPoints.erase(shuffledPoints.begin());
    k--;

    int id;
    double dist;
    double distances[size];

    while (k > 0) {
        //initialize distances to INF
        for (int i = 0; i < size; i++) {
            distances[i] = DBL_MAX;
        }

        //loop through all points and find distance to closest centroid for each one
        for (auto point: shuffledPoints) {
            id = stoi(point->getId());
            for (auto centroid: centroids) {

                dist = point->l2Distance(centroid);
                
                dist = dist * dist;

                if (dist < distances[id]) {
                    distances[id] = dist;
                }

            }
        }

        //add all closest distances together
        double totalDists = 0;
        for (int i = 0; i < size; i++) {
            if (distances[i] != DBL_MAX) {
                totalDists += distances[i];
            }
        }

        //Choose a random float from 0-1
        int idx = 0;
        float chosen = getUniformRandomFloat();

        //reduce it by dist[point]/totalDists (normalized probability)
        //this way, when the number becomes negative we stop, and we
        //have randomly selected a point with the required probability
        while (chosen > 0) {
            if (distances[idx] != DBL_MAX) {
                //std::cout << chosen << std::endl;
                chosen -= distances[idx] / totalDists;
            }
            idx++;
        }

        idx--;

        //get the id of the point so we can delete it from the list
        std::string targetId = shuffledPoints[idx]->getId();

        //add it to the centroids list
        centroids.push_back(shuffledPoints[idx]);

        //delete it from the points list
        std::vector<Point *>::iterator it;
        for (it = shuffledPoints.begin(); it != shuffledPoints.end(); it++) {
            if ((*it)->getId() == targetId) {
                shuffledPoints.erase(it);
                break;
            }
        }
        k--;
    }
    return 0;
}

double Kmeans::calculatePointSilhouette(Point *point) {
    double a = 0;
    double b = 0;   


    //calculate average distance from other points of the same cluster
    //std::cout<<point->getClusterIndex();

    for (auto p: clusters[point->getClusterIndex()].getClusteredPoints()) {
        if (p->getId() == point->getId())continue;
        a += point->l2Distance(p);
    }
    int clusterSize = (clusters[point->getClusterIndex()].getClusteredPoints().size());
    if (clusterSize - 1 == 0) {
        a = 0;
    } else {
        a /= clusterSize - 1;
    }
    

    double minDistance = DBL_MAX;
    unsigned int minIndex = -1;
    double tempDistance = 0;

    //calculate average distance from points of the next best cluster
    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        if ((unsigned int) point->getClusterIndex() == i)continue;
        tempDistance = point->l2Distance(&(clusters[i].getCentroid()));
        if (tempDistance <= minDistance) {
            minDistance = tempDistance;
            minIndex = i;
        }
    }

    for (auto p: clusters[minIndex].getClusteredPoints()) {
        b += point->l2Distance(p);
    }

    clusterSize = (clusters[minIndex].getClusteredPoints().size());

    if (clusterSize != 0) {
        b /= clusterSize;
    } else {
        b = 0;
    }

    //calculate silhouette
    double silhouette;
    (a != 0 || b != 0) ? silhouette = (b - a) / std::max(a, b) : silhouette = 0;

    return silhouette;

}

void Kmeans::displaySilhouette(FILE* fp) {
    std::cout << std::endl << "-----------------------------------------------" << std::endl;
    std::cout << "                SILHOUETTES                    " << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;

    if(this->numOfClusters == 1){
        std::cout << "Only 1 cluster - Silhouette value cannot be computed" << std::endl;
        return;
    }
    else if(this->numOfClusters <= 0){
        std::cout << "Invalid number of clusters (<= 0)" << std::endl;
        return;
    }

    double averageSilhouettes[this->numOfClusters];

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        averageSilhouettes[i] = 0;
    }
    double averageTotalSilhouette = 0;
    double sil = 0;

    for (auto p: this->points) {
        if(p->getClusterIndex() == -1)continue;
        sil = this->calculatePointSilhouette(p);
        std::cout << "Silhouette: " << sil << std::endl;
        averageTotalSilhouette += sil;
        averageSilhouettes[p->getClusterIndex()] += sil;
    }
    double clusterSize;
    fprintf(fp, "Silhouette: [");
    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        clusterSize = this->clusters[i].getClusteredPoints().size();
        if (clusterSize == 0) {
            fprintf(fp, "NaN");
        } else {
            averageSilhouettes[i] /= clusterSize;
            fprintf(fp, "%lf,", averageSilhouettes[i]);
        }
    }

    averageTotalSilhouette = averageTotalSilhouette / this->points.size();
    std::cout << std::endl << "Average Silhouette: " << averageTotalSilhouette << std::endl;
    fprintf(fp, "%lf]\n", averageTotalSilhouette);
}

void Kmeans::printClusters(FILE* fp){
    for(unsigned int i = 0; i < this->numOfClusters; i++){
        fprintf(fp, "CLUSTER-%d {size: %d, centroid: [", i, (int)this->clusters[i].getClusteredPoints().size());
        for(auto item: this->clusters[i].getCentroid().getVector()){
            fprintf(fp, "%.3lf ", item);
        }
        fprintf(fp, "] }\n");
    }
}

void Kmeans::printCompleteInfo(FILE* fp){
    for(unsigned int i = 0; i < this->numOfClusters; i++){
        fprintf(fp, "CLUSTER-%d {centroid, ",i);
        for(auto point: this->clusters[i].getClusteredPoints()){
            fprintf(fp, "%s, ", point->getId().c_str());
        }
        fseek(fp, -2, SEEK_CUR);
        fprintf(fp,"] \n");
    }
}

double Kmeans::calculateInitialRadius(){
    double minDistance = DBL_MAX;
    double tempDistance;
    for(unsigned int i  = 0; i < numOfClusters; i++){
        for(unsigned int j = 0; j < numOfClusters; j++){
            if(i==j)continue;
            tempDistance = clusters[j].getCentroid().l2Distance(clusters[i].getCentroid().getVector());
            if(tempDistance < minDistance){
                minDistance = tempDistance;
            }
        }
    }
    return minDistance/2;
}