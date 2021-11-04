#include <cfloat>
#include <algorithm>
#include <cmath>
#include "../inc/kmeans.hpp"
#include "../inc/randGen.hpp"
#include "../inc/LSH.hpp"
#include "../inc/hypercube.hpp"
#include "../inc/readInput.hpp"

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

    std::vector <Point *> centroids;

    std::cout << "Attempting to find centroids\n";

    if (method == Random) {
        this->findRandomCentroids(points, this->numOfClusters, centroids);
    } else if (method == PlusPlus) {
        this->findPlusPlusCentroids(points, this->numOfClusters, centroids);
    } else {
        return -1;
    }
    std::cout << "Found centroids\n";

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        this->clusters[i].setCentroid(centroids[i]);
    }
    return 0;
}

int Kmeans::computeLSH(double maxRadius, unsigned int maxIters, centroidInitializationMethod method) {

    int radius = 200;

    std::cout << "Appending points in lsh structure\n";
    
    LSH* lsh = new LSH(128, 1000, 10, 4, 1000); //TODO pass these or read form config etc

    std::cout << "Done\n";

    for(auto point: this->points){
        lsh->addPoint(point);
    }

    std::cout << "Initializing centroids\n";

    if(initializeCentroids(lsh->getPoints(), method) < 0) return -1;

    unsigned int iter;
    double sumOfCentroidMoves = DBL_MAX;

    std::cout << "Clearing clusters\n";

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        this->clusters[i].clearList();
    }

    for (iter = 0; iter < maxIters; iter++) {

        for(unsigned int i = 0; i < this->numOfClusters; i++){
            //std::cout << "Calculating points for cluster i\n";
            lsh->getNearestByR(radius, iter, this->clusters, i);
        }

        //if (sumOfCentroidMoves < iterThreshold)break;
        //sumOfCentroidMoves = 0;

        for (unsigned int i = 0; i < this->numOfClusters; i++) {
            sumOfCentroidMoves += this->clusters[i].recenter();
        }

        radius *= 2;
        if(radius >= maxRadius)break;
        std::cout << radius << std::endl;

    }

    double minDistance;
    double distance;
    int minIndex;

    for(auto p : lsh->getPoints()){
        int idx = p->getClusterIndex();
        if(idx != -1){
            this->clusters[idx].insertPoint(p);
        }
        else{
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


    unsigned int pointsAssigned = 0;

    std::cout << "Total Iterations: " << iter << std::endl;

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        unsigned int clusterPoints = clusters[i].count();
        std::cout << "Items in cluster " << i << ": " << clusterPoints << std::endl;
        pointsAssigned += clusterPoints; 
    }

    std::cout << "Total points assigned: " << pointsAssigned << std::endl;

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

int Kmeans::computeHypercube(double maxRadius, unsigned int maxIters, centroidInitializationMethod method) {

    int radius = 200;

    std::cout << "Appending points in lsh structure\n";

    Hypercube* hypercube = new Hypercube(128, (int)pow(2,14), 1, 14, 1000); //TODO pass these or read form config etc

    std::cout << "Done\n";

    for(auto point: this->points){
        hypercube->addPoint(point);
    }

    std::cout << "Initializing centroids\n";

    if(initializeCentroids(this->points, method) < 0) return -1;

    unsigned int iter;
    double sumOfCentroidMoves = DBL_MAX;

    std::cout << "Clearing clusters\n";

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        this->clusters[i].clearList();
    }

    for (iter = 0; iter < maxIters; iter++) {

        for(unsigned int i = 0; i < this->numOfClusters; i++){
            //std::cout << "Calculating points for cluster i\n";
            hypercube->getNearestByR(radius, iter, this->clusters, i);
        }

        //if (sumOfCentroidMoves < iterThreshold)break;
        //sumOfCentroidMoves = 0;

        for (unsigned int i = 0; i < this->numOfClusters; i++) {
            sumOfCentroidMoves += this->clusters[i].recenter();
        }

        radius *= 2;
        if(radius >= maxRadius)break;
        std::cout << radius << std::endl;

    }

    double minDistance;
    double distance;
    int minIndex;

    for(auto p : this->points){
        int idx = p->getClusterIndex();
        if(idx != -1){
            this->clusters[idx].insertPoint(p);
        }
        else{
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


    unsigned int pointsAssigned = 0;

    std::cout << "Total Iterations: " << iter << std::endl;

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        unsigned int clusterPoints = clusters[i].count();
        std::cout << "Items in cluster " << i << ": " << clusterPoints << std::endl;
        pointsAssigned += clusterPoints;
    }

    std::cout << "Total points assigned: " << pointsAssigned << std::endl;

    return 0;
}

void Kmeans::findRandomCentroids(std::list<Point *> &points, unsigned int k, std::vector<Point *> &centroids) {
    int size = points.size();
    std::cout << size << std::endl;

    std::vector < Point * > shuffledPoints(points.begin(), points.end());
    std::random_shuffle(std::begin(shuffledPoints), std::end(shuffledPoints));
    for (unsigned int i = 0; i < k; i++) {
        centroids.push_back(shuffledPoints[i]);
    }
}

void Kmeans::findPlusPlusCentroids(std::list<Point *> &points, unsigned int k, std::vector<Point *> &centroids) {
    int size = points.size();
    //std::cout << size << std::endl;
    std::vector < Point * > shuffledPoints(points.begin(), points.end());
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

                //square dist TODO
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

void Kmeans::displaySilhouette() {
    std::cout << std::endl << "-----------------------------------------------" << std::endl;
    std::cout << "                SILHOUETTES                    " << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;

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
    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        clusterSize = this->clusters[i].getClusteredPoints().size();
        if (clusterSize == 0) {
            std::cout << "Cluster " << i << ": No Points In This Cluster" << std::endl;
        } else {
            averageSilhouettes[i] /= clusterSize;
            std::cout << "Cluster " << i << ": " << averageSilhouettes[i] << std::endl;
        }
    }

    averageTotalSilhouette = averageTotalSilhouette / this->points.size();
    std::cout << std::endl << "Average Silhouette: " << averageTotalSilhouette << std::endl;
}