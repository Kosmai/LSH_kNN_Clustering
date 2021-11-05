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

    int initializeCentroids(std::list<Point *> &points, centroidInitializationMethod method);

    void findRandomCentroids(std::list<Point *> &points, unsigned int k, std::vector<Point *> &centroids);

    void findPlusPlusCentroids(std::list<Point *> &points, unsigned int k, std::vector<Point *> &centroids);

public:
    Kmeans() = delete;

    Kmeans(unsigned int dimension, unsigned int numOfClusters);

    ~Kmeans();

    int addPoint(Point *point);

    int computeLoyd(double iterThreshold, unsigned int maxIters, centroidInitializationMethod);

    int computeLSH(double maxRadius, unsigned int maxIters, centroidInitializationMethod, int buckets, int L, int k, int w);

    int computeHypercube(double maxRadius, unsigned int maxIters, centroidInitializationMethod, int d, int w, int probes, int M);

    double calculatePointSilhouette(Point *point);

    void displaySilhouette();
};
