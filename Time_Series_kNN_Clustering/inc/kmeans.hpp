#pragma once

#include <vector>
#include <list>

enum centroidInitializationMethod {
    Random = 0, PlusPlus
};

class Cluster;
class Point;
class LSH;

/* This module provides the Kmeans class which is used to store basic parameters and
functionality for all the Kmeans clustering implementations used (Lloyd, Reverse
assignment using LSH/Hypercube) */

class Kmeans {
private:
    unsigned int dimension;
    unsigned int numOfClusters;
    int metric;
    std::list<Point *> points;
    Cluster *clusters;
    LSH *lsh;

    //assigns centroids to all clusters using the given method (Kmeans++ or random assigment)
    int initializeCentroids(std::list<Point *> &points, centroidInitializationMethod method, int metric = 0);
    //called by initializeCentroids to randomly assign centroids
    int findRandomCentroids(std::list<Point *> &points, unsigned int k, std::vector<Point *> &centroids);
    //called by initializeCentroids to assign centroids using the Kmeans++ initialization method
    int findPlusPlusCentroids(std::list<Point *> &points, unsigned int k, std::vector<Point *> &centroids, int metric = 0);
    //used in reverse assignment to compute optimal starting radius
    double calculateInitialRadius();
    
public:
    //constructors-destructors
    Kmeans() = delete;
    Kmeans(unsigned int dimension, unsigned int numOfClusters, int metric);
    ~Kmeans();

    //adds a point to the structure
    int addPoint(Point *point);
    //clusters all points using Lloyd's algorithm
    int computeLoyd(double iterThreshold, unsigned int maxIters, centroidInitializationMethod, int metric = 0);
    //clusters all points using reverse assignemnt with LSH algorithm
    int computeLSH(double maxRadius, unsigned int maxIters, centroidInitializationMethod, int buckets, int L, int k, int w, int metric = 0);
    //clusters all points using reverse assignemnt with Hypercube algorithm
    int computeHypercube(double maxRadius, unsigned int maxIters, centroidInitializationMethod, int d, int w, int probes, int M);
    //returns the silhouette value of the point given
    double calculatePointSilhouette(Point *point, int metric = 0);
    //writes all the silhoutte values needed in the fp output file
    void displaySilhouette(FILE* fp, int metric = 0);
    //writes clusters in output file fp
    void printClusters(FILE* fp);
    //writes additional info to output file fp
    void printCompleteInfo(FILE* fp);
};
