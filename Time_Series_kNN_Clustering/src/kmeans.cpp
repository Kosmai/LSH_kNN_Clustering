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
#include "../inc/timeSeries.hpp"

Kmeans::Kmeans(unsigned int dimension, unsigned int numOfClusters, int metric) : dimension(dimension),
                                                                     numOfClusters(numOfClusters), metric(metric) {
    clusters = new Cluster[numOfClusters];
    lsh = nullptr;

    for (unsigned int i = 0; i < numOfClusters; i++) {
        clusters[i] = Cluster(dimension);
    }
}

Kmeans::~Kmeans() {
    for(unsigned int i = 0; i < numOfClusters; i++){
        delete clusters[i].getCentroid().getTimeSeries();
    }
    delete[] clusters;
    if(metric == 1 && lsh != nullptr){
        for(auto lshPoint: lsh->getPoints()){
            delete lshPoint;
        }
    }
    delete lsh;
}

int Kmeans::addPoint(Point *point) {
    this->points.push_back(point);
    return 0;
}

int Kmeans::initializeCentroids(std::list<Point *> &points, centroidInitializationMethod method, int metric){

    //make sure there are less clusters than points
    if(this->numOfClusters > points.size() || this->numOfClusters <= 0){
        return -1;
    }

    std::vector <Point *> centroids;

    std::cout << "Centroid intialization...";

    //determine which initialization method will be used
    if (method == Random) {
        if(this->findRandomCentroids(points, this->numOfClusters, centroids) < 0) return -1;
    } else if (method == PlusPlus) {
        if(this->findPlusPlusCentroids(points, this->numOfClusters, centroids, metric) < 0) return -1;
    } else {
        return -1;
    }
    std::cout << "done\n";

    //assign each cluster a centroid
    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        this->clusters[i].setCentroid(centroids[i]);
    }
    return 0;
}

int Kmeans::computeLoyd(double iterThreshold, unsigned int maxIters, centroidInitializationMethod method, int metric) {

    if(initializeCentroids(this->points, method, metric) < 0) return -1;

    double dist, minDist;
    unsigned int minIndex;

    unsigned int iter;
    double sumOfCentroidMoves = DBL_MAX;

    for (iter = 0; iter < maxIters; iter++) {

        //clears any previously assigned points in clusters
        for (unsigned int i = 0; i < this->numOfClusters; i++) {
            this->clusters[i].clearList();
        }

        //compute closest centroid for every point
        for (auto point: points) {
            minDist = DBL_MAX;
            minIndex = -1;
            for (unsigned i = 0; i < this->numOfClusters; i++) {
                if(metric == 0){
                    dist = clusters[i].getCentroid().l2Distance(point);
                }
                else if(metric == 1){
                    dist = clusters[i].getCentroid().getTimeSeries()->discreteFrechetDistance(point->getTimeSeries()->getVector());
                }
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

        //recenter the centroids based on the average distance from their points
        for (unsigned int i = 0; i < this->numOfClusters; i++) {
            sumOfCentroidMoves += this->clusters[i].recenter(metric);
        }

        std::cout << "Total centroid movement: " << sumOfCentroidMoves << std::endl;

    }

    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "Total Iterations: " << iter << std::endl;

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        std::cout << "Items in cluster " << i << ": " << clusters[i].count() << std::endl;
    }

    return 0;
}

int Kmeans::computeLSH(double maxRadius, unsigned int maxIters, centroidInitializationMethod method,
                       int buckets, int L, int k, int w, int metric) {

    double minDistance;
    double distance;
    int minIndex;
    double centroidMove;
    
    //initialize lsh structure
    if(metric == 0){
        lsh = new LSH(this->dimension, buckets, L, k, w);
        for(auto point: this->points){
            lsh->addPoint(point);
        }
    }
    else if(metric == 1){
        lsh = new LSH(this->dimension*2, buckets, L, k, w);
        for(auto point: this->points){
            lsh->addTimeSeries(point->getTimeSeries(), 1, 1);
        }
    }
    else{
        return -1;
    }
    

    if(initializeCentroids(this->points, method, metric) < 0) return -1;

    int radius = calculateInitialRadius();

    unsigned int iter;
    double sumOfCentroidMoves = DBL_MAX;

    //clears any previously assigned points in clusters
    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        this->clusters[i].clearList();
    }

    for (iter = 0; iter < maxIters; iter++) {

        //for every centroid find near neighbor points
        for(unsigned int i = 0; i < this->numOfClusters; i++){
            lsh->getNearestByR(radius, this->clusters, i, metric);
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
        }

        sumOfCentroidMoves = 0;

        //recenter centroids
        for (unsigned int i = 0; i < this->numOfClusters; i++) {
            centroidMove = this->clusters[i].recenter(metric);
            if(centroidMove == -1){
                centroidMove = 0;
            }
            sumOfCentroidMoves += centroidMove;
        }
        std::cout << "Total centroid movement: " << sumOfCentroidMoves << std::endl;

        radius *= 2;
        if(radius >= maxRadius)break;
        std::cout << "Max radius: " << radius << std::endl;

    }
    int unassignedPoints = 0;

    //assign unassigned points using brute force
    for(auto p : lsh->getPoints()){
        if(p->getClusterIndex() == -1){
            unassignedPoints++;
            minDistance = DBL_MAX;
            minIndex = -1;

            for(unsigned int i = 0; i < this->numOfClusters; i++){
                if(metric == 0){
                    distance = p->l2Distance(this->clusters[i].getCentroid().getVector());
                }
                else if(metric == 1){
                    distance = p->getTimeSeries()->discreteFrechetDistance(this->clusters[i].getCentroid().getTimeSeries());
                }
                if(distance <= minDistance){
                    minDistance = distance;
                    minIndex = i;
                }
            }   
            p->setClusterIndex(minIndex);
            this->clusters[minIndex].insertPoint(p);
        }
    }

    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "Unassigned points using LSH: " <<  unassignedPoints << std::endl;

    //final recenter
    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        sumOfCentroidMoves += this->clusters[i].recenter(metric);
    }


    unsigned int pointsAssigned = 0;

    std::cout << "Total Iterations: " << iter << std::endl;

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        unsigned int clusterPoints = clusters[i].count();
        std::cout << "Items in cluster " << i << ": " << clusterPoints << std::endl;
        pointsAssigned += clusterPoints; 
    }

    for(auto lshPoint: lsh->getPoints()){
        for(auto originalPoint: this->points){
            if(lshPoint->getId() == originalPoint->getId()){
                originalPoint->setClusterIndex(lshPoint->getClusterIndex());
            }
        }
    }

    return 0;
}



int Kmeans::computeHypercube(double maxRadius, unsigned int maxIters, centroidInitializationMethod method,
                             int d, int w, int probes, int M) {

    double minDistance;
    double distance;
    int minIndex;
    double centroidMove;

    //initialize hypercube structure
    Hypercube* hypercube = new Hypercube(this->dimension, (int)pow(2,d), 1, d, w);

    for(auto point: this->points){
        hypercube->addPoint(point);
    }

    if(initializeCentroids(this->points, method) < 0) return -1;

    int radius = calculateInitialRadius();

    unsigned int iter;
    double sumOfCentroidMoves = DBL_MAX;

    //clear cluster points
    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        this->clusters[i].clearList();
    }

    for (iter = 0; iter < maxIters; iter++) {
        //for every centroid find near neighbor points
        for(unsigned int i = 0; i < this->numOfClusters; i++){
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
        }

        sumOfCentroidMoves = 0;
        
        //recenter centroids
        for (unsigned int i = 0; i < this->numOfClusters; i++) {
            centroidMove = this->clusters[i].recenter();
            if(centroidMove == -1){
                centroidMove = 0;
            }
            sumOfCentroidMoves += centroidMove;
        }

        std::cout << "Total centroid movement: " << sumOfCentroidMoves << std::endl;

        radius *= 2;
        if(radius >= maxRadius)break;
        std::cout << "Max radius: " << radius << std::endl;

    }

    int unassignedPoints = 0;

    //assign unassigned points using brute force
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

    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "Unassigned points using Hypercube: " <<  unassignedPoints << std::endl;

    //final recenter
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
        //printf("%p\n", centroids[centroids.size()-1]->getTimeSeries());
    }
    return 0;
}

int Kmeans::findPlusPlusCentroids(std::list<Point *> &points, unsigned int k, std::vector<Point *> &centroids, int metric) {
    int size = points.size();
    if(size <= 0){
        return -1;
    }

    std::vector <Point *> shuffledPoints(points.begin(), points.end());
    std::random_shuffle(std::begin(shuffledPoints), std::end(shuffledPoints));
    centroids.push_back(shuffledPoints[0]); //assign 1 centroid at random
    shuffledPoints.erase(shuffledPoints.begin());
    k--;

    double dist;
    std::map<std::string,double> distances;

    while (k > 0) {
        //initialize distances to INF
        for(auto point: points){
            distances[point->getId()] = DBL_MAX;
        }

        //loop through all points and find distance to closest centroid for each one
        for (auto point: shuffledPoints) {
            for (auto centroid: centroids) {

                if(metric == 0){
                    dist = point->l2Distance(centroid);
                }
                else if(metric == 1){
                    dist = point->getTimeSeries()->discreteFrechetDistance(centroid->getTimeSeries()->getVector());
                }
                
                dist = dist * dist;

                if (dist < distances[point->getId()]) {
                    distances[point->getId()] = dist;
                }

            }
        }

        //add all closest distances together
        double totalDists = 0;
        for (auto point: points) {
            if (distances[point->getId()] != DBL_MAX) {
                totalDists += distances[point->getId()];
            }
        }

        //Choose a random float from 0-1
        float chosen = getUniformRandomFloat();

        //reduce it by dist[point]/totalDists (normalized probability)
        //this way, when the number becomes negative we stop, and we
        //have randomly selected a point with the required probability

        std::string chosenId;
        Point* chosenPoint;

        for (auto distance: distances){
            if (distance.second != DBL_MAX) {
                chosen -= distance.second / totalDists;
                if(chosen <= 0){
                    chosenId = distance.first;
                    break;
                }
            }
        }

        for(auto point: shuffledPoints){
            if(point->getId() == chosenId){
                chosenPoint = point;
                break;
            }
        }

        //add it to the centroids list
        centroids.push_back(chosenPoint);

        //delete it from the points list
        std::vector<Point *>::iterator it;
        for (it = shuffledPoints.begin(); it != shuffledPoints.end(); it++) {
            if ((*it)->getId() == chosenId) {
                shuffledPoints.erase(it);
                break;
            }
        }
        k--;
    }
    return 0;
}

double Kmeans::calculatePointSilhouette(Point *point, int metric) {
    double a = 0;
    double b = 0;   


    //calculate average distance from other points of the same cluster
    for (auto p: clusters[point->getClusterIndex()].getClusteredPoints()) {
        if (p->getId() == point->getId())continue;
        if(metric == 0){
            a += point->l2Distance(p);
        }
        else if(metric == 1){
            a += point->getTimeSeries()->discreteFrechetDistance(p->getTimeSeries());
        }
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
        if(metric == 0){
            tempDistance = point->l2Distance(&(clusters[i].getCentroid()));
        }
        else if(metric == 1){
            tempDistance = point->getTimeSeries()->discreteFrechetDistance(clusters[i].getCentroid().getTimeSeries());
        }

        if (tempDistance <= minDistance) {
            minDistance = tempDistance;
            minIndex = i;
        }
    }

    for (auto p: clusters[minIndex].getClusteredPoints()) {
        if(metric == 0){
            b += point->l2Distance(p);
        }
        else if (metric == 1){
            b += point->getTimeSeries()->discreteFrechetDistance(p->getTimeSeries());
        }

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

void Kmeans::displaySilhouette(FILE* fp, int metric) {
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "Silhouette" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "Calculating silhouette - This may take a long time..." << std::endl;

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
        sil = this->calculatePointSilhouette(p, metric);
        std::cout<<"SIL:"<<sil<<std::endl;
        //std::cout << "Silhouette: " << sil << std::endl;
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

    //calculate min distance of all clusters
    for(unsigned int i  = 0; i < numOfClusters; i++){
        for(unsigned int j = 0; j < numOfClusters; j++){
            if(i==j)continue;
            tempDistance = clusters[j].getCentroid().l2Distance(clusters[i].getCentroid().getVector());
            if(tempDistance < minDistance){
                minDistance = tempDistance;
            }
        }
    }

    //return minDistance/2 if possible
    return minDistance>2 ? minDistance/2 : 2;
}