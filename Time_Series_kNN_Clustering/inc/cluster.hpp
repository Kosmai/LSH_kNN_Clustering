#pragma once

#include <list>
#include "../inc/point.hpp"

/* This module includes class Cluster which represents a
 cluster by storing pointers to the assigned points and
 the position of the cluster's centroid*/

class Cluster {
private:
    int dimension;
    std::list<Point *> clusteredPoints;
    Point centroid;

public:
    //constructors-destructors
    Cluster() = default;
    Cluster(int dimension);
    ~Cluster() = default;
    Cluster(const Cluster &copy) = default;
    Cluster &operator=(const Cluster &copy);

    //appends given point to cluster list
    int insertPoint(Point* point);

    //clears cluster list
    int clearList();

    //getters-setters
    int setCentroid(Point* centroid);
    Point &getCentroid();
    std::list<Point *>& getClusteredPoints();

    //sets centroid as the average of cluster's
    //points and returns how much it moved
    double recenter(int metric = 0);

    //used for testing purposes
    int count();
    void print();
};