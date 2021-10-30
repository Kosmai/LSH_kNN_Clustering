#pragma once

#include "point.hpp"
#include <list>

class Cluster {
private:
    int dimension;
    std::list<Point *> clusteredPoints;
    Point centroid;

public:

    Cluster() = default;

    Cluster(int dimension);

    ~Cluster();

    Cluster(const Cluster &copy) = default;

    Cluster &operator=(const Cluster &copy);

    int insertPoint(Point* point);

    int clearList();

    int setCentroid(Point* centroid);

    Point &getCentroid();

    int count();

    std::list<Point *> getClusteredPoints();

    double recenter();

    void print();

};