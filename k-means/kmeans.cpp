#include "kmeans.hpp"
#include <cfloat>
#include "randGen.hpp"
#include <algorithm>

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

int Kmeans::computeLoyd(double iterThreshold, unsigned int maxIters, centroidInitializationMethod method) {
    std::vector < Point * > centroids;

    if (method == Random) {
        this->findRandomCentroids(this->points, this->numOfClusters, centroids);
    } else if (method == PlusPlus) {
        this->findPlusPlusCentroids(this->points, this->numOfClusters, centroids);
    } else {
        return -1;
    }

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        this->clusters[i].setCentroid(centroids[i]);
    }

    double dist, minDist;
    unsigned int minIndex;

    unsigned int iter;

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

        double sumOfCentroidMoves = 0;

        for (unsigned int i = 0; i < this->numOfClusters; i++) {
            sumOfCentroidMoves += this->clusters[i].recenter();
        }

        std::cout << sumOfCentroidMoves << std::endl;
        if (sumOfCentroidMoves < iterThreshold)break;
    }

    std::cout << "Total Iterations: " << iter << std::endl;

    for (unsigned int i = 0; i < this->numOfClusters; i++) {
        std::cout << "Items in cluster " << i << ": " << clusters[i].count() << std::endl;
        //clusters[i].print();
    }

    return 0;
}

int Kmeans::computeLSH() {
    return 0;
}

int Kmeans::computeHypercube() {
    return 0;
}

void Kmeans::findRandomCentroids(std::list<Point *> &points, unsigned int k, std::vector<Point *> &centroids) {
    //int size = points.size();
    //std::cout << size << std::endl;

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
    for (auto p: clusters[point->getClusterIndex()].getClusteredPoints()) {
        if (p->getId() == point->getId())continue;
        a += point->l2Distance(p);
    }

    a /= (clusters[point->getClusterIndex()].getClusteredPoints().size() - 1);


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

    b /= (clusters[minIndex].getClusteredPoints().size());

    //calculate silhouette
    double silhouette = (b - a) / std::max(a, b);

    return silhouette;

}

void Kmeans::displaySilhouette() {
    double totalSilhouette = 0;
    double sil = 0;

    int counter=0;
    for (auto p: this->points) {
        counter++;
        sil = this->calculatePointSilhouette(p);
        std::cout << "Silhouette: " << sil << std::endl;
        totalSilhouette += sil;
    }

    double averageSilhouette = totalSilhouette/this->points.size();
    std::cout << std::endl << "Average Silhouette: " << averageSilhouette << std::endl;
}