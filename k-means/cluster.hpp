#pragma once

#include "point.hpp"
#include <list>

class Cluster {
private:
    int dimension;
    std::list<Point *> clusteredPoints;
    Point* centroid;

public:
    Cluster(int dimension);

    ~Cluster();

    Cluster(const Cluster &copy) = default;

    Cluster &operator=(const Cluster &copy);

    int insertPoint(Point* point);

    int clearList();

    int setCentroid(Point* centroid);

    Point &getCentroid();

    std::list<Point *> getClusteredPoints();

    int recenter();

    void print();

};