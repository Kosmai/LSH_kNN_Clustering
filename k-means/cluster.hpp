#pragma once

#include "point.hpp"
#include <list>

class Cluster {
private:
    std::list<Point *> clusteredPoints;
    Point centroid;

public:
    Cluster();

    ~Cluster();

    Cluster(const Cluster &copy) = default;

    Cluster &operator=(const Cluster &copy);

    int recenter();

    void display();

};