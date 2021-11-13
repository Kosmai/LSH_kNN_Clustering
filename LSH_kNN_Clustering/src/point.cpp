#include <iostream>
#include <cmath>
#include "../inc/point.hpp"

Point::Point() {
    this->id = "Uninitialized";
    this->clusterIndex = -1;
}

Point::Point(std::vector<double> &vec, const std::string &id) {
    //copy constructor
    this->vec = vec;
    this->id = id;
    this->clusterIndex = -1;
}

Point &Point::operator=(const Point &copy) {
    //copy constructor
    this->vec = copy.vec;
    this->id = copy.id;
    this->clusterIndex = copy.clusterIndex;

    return *this;
}

int Point::setId(std::string &id) {
    this->id = id;
    return 0;
}

int Point::setVector(std::vector<double> &vec) {
    this->vec = vec;
    return 0;
}

int Point::setClusterIndex(int clusterIndex) {
    this->clusterIndex = clusterIndex;
    return 0;
}

std::string Point::getId() {
    return this->id;
}

std::vector<double> &Point::getVector() {
    return this->vec;
}

int Point::getClusterIndex() {
    return this->clusterIndex;
}

int Point::getDimension() {
    return this->vec.size();
}

bool Point::operator==(Point &p) {
    return this->id == p.id;
}

double Point::l2Distance(Point *otherPoint) {
    //check if dimensions differ
    if (otherPoint->getDimension() != this->getDimension()) {
        return -1;
    }

    //calculate L2 distance (euclidean)
    double sum = 0;
    for (int i = 0; i < this->getDimension(); i++) {
        sum += (this->vec[i] - otherPoint->getVector()[i]) * (this->vec[i] - otherPoint->getVector()[i]);
    }
    return sqrt(sum);
}

double Point::l2Distance(std::vector<double> &otherVector) {
    //check if dimensions differ
    if ((int) otherVector.size() != this->getDimension()) {
        return -1;
    }

    //calculate L2 distance (euclidean)
    double sum = 0;
    for (int i = 0; i < this->getDimension(); i++) {
        sum += (this->vec[i] - otherVector[i]) * (this->vec[i] - otherVector[i]);
    }
    return sqrt(sum);
}

void Point::print() {
    std::cout << "Point " << this->id << std::endl << "=================" << std::endl;
    for (auto i: this->vec) {
        std::cout << "|-- > " << i << std::endl;
    }
    std::cout << "=================" << std::endl;
}

void Point::printInfo() {
    std::cout << "Point " << this->id << std::endl;
    std::cout << "  Cluster: " << this->clusterIndex << std::endl;

}