#include <cmath>
#include "../inc/point.hpp"

Point::Point() {
    this->id = "Uninitialized";
    this->clusterIndex = -1;
    this->rangeIndex = -1;
}

Point::Point(std::vector<double> &vec, const std::string &id) {
    this->vec = vec; //copy construct
    this->id = id;
    this->clusterIndex = -1;
    this->rangeIndex = -1;
}

Point &Point::operator=(const Point &copy) {
    this->vec = copy.vec; //copy construct
    this->id = copy.id;
    this->clusterIndex = copy.clusterIndex;
    this->rangeIndex = copy.rangeIndex;

    return *this;
}

int Point::setId(std::string &id) {
    this->id = id;

    return 0;
}

int Point::setVector(std::vector<double> &vec) {
    this->vec = vec; //copy construct

    return 0;
}

int Point::setClusterIndex(int clusterIndex) {
    this->clusterIndex = clusterIndex;
    return 0;
}

int Point::setRangeIndex(int rangeIndex) {
    this->rangeIndex = rangeIndex;
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

int Point::getRangeIndex() {
    return this->rangeIndex;
}

int Point::getDimension() {
    return this->vec.size();
}

bool Point::operator==(Point &p) {
    return this->id == p.id;
}

double Point::l2Distance(Point *otherPoint) {
    if (otherPoint->getDimension() != this->getDimension()) {
        return -1;
    }

    double sum = 0;
    for (int i = 0; i < this->getDimension(); i++) {
        sum += (this->vec[i] - otherPoint->getVector()[i]) * (this->vec[i] - otherPoint->getVector()[i]);
    }
    return sqrt(sum);
}

double Point::l2Distance(std::vector<double> &otherVector) {
    if ((int) otherVector.size() != this->getDimension()) {
        return -1;
    }

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
    std::cout << "  Range: " << this->rangeIndex << std::endl;

}