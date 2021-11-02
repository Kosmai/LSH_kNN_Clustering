#pragma once

#include "../inc/cluster.hpp"
#include <vector>

enum centroidInitializationMethod {
    Random = 0, PlusPlus
};

class Kmeans {
private:
    unsigned int dimension;
    unsigned int numOfClusters;
    std::list<Point *> points;
    Cluster *clusters;

    void findRandomCentroids(std::list<Point *> &points, unsigned int k, std::vector<Point *> &centroids);

    void findPlusPlusCentroids(std::list<Point *> &points, unsigned int k, std::vector<Point *> &centroids);

public:
    Kmeans() = delete;

    Kmeans(unsigned int dimension, unsigned int numOfClusters);

    ~Kmeans();

    int addPoint(Point *point);

    int computeLoyd(double iterThreshold, unsigned int maxIters, centroidInitializationMethod);

    int computeLSH();

    int computeHypercube();

    double calculatePointSilhouette(Point *point);

    void displaySilhouette();
};
