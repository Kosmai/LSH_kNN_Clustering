#include "cluster.hpp"

Cluster::Cluster() {

}

Cluster::~Cluster() {

}

Cluster &Cluster::operator=(const Cluster &copy) {
    this->clusteredPoints = copy.clusteredPoints;
    this->centroid = copy.centroid;

    return *this;
}

int Cluster::recenter() {
    return 0;
}

void Cluster::display() {}