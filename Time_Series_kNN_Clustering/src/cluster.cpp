#include <iostream>

#include "../inc/cluster.hpp"
#include "../inc/point.hpp"
#include "../inc/timeSeries.hpp"

Cluster::Cluster(int dimension) {
    this->dimension = dimension;
}

Cluster &Cluster::operator=(const Cluster &copy) {
    this->clusteredPoints = copy.clusteredPoints;
    this->centroid = copy.centroid;
    this->dimension = copy.dimension;

    return *this;
}

int Cluster::insertPoint(Point *point) {
    clusteredPoints.push_back(point);
    return 0;
}

int Cluster::clearList() {
    clusteredPoints.clear();
    return 0;
}

int Cluster::setCentroid(Point *centroid) {
    if(centroid->getDimension() != this->dimension){
        return -1;
    }
    this->centroid = *centroid;
    TimeSeries* ts = new TimeSeries(centroid);
    this->centroid.setTimeSeries(ts);
    return 0;
}

Point& Cluster::getCentroid() {
    return this->centroid;
}

int Cluster::count() {
    return this->clusteredPoints.size();
}

std::list<Point *>& Cluster::getClusteredPoints(){
    return this->clusteredPoints;
}

double Cluster::recenter(int metric) {

    //if cluster has no points return -1
    if (!this->clusteredPoints.size()) {
        return -1;
    }
    double moved;
    if(metric == 0){
        //calculate average vector/point and set it as new centroid
        std::vector<double> average(this->dimension, 0);
        for (auto v: this->clusteredPoints) {
            for (int i = 0; i < this->dimension; i++) {
                average[i] += v->getVector()[i];
            }
        }

        for (int i = 0; i < this->dimension; i++) {
            average[i] /= this->clusteredPoints.size();
        }

        moved = this->centroid.l2Distance(average);
        this->centroid.setVector(average);
    }
    else if(metric == 1) {
        std::vector <std::vector<Observation>> clusterCurves;
        for (auto point: this->clusteredPoints) {
            clusterCurves.push_back(point->getTimeSeries()->getVector());
        }
        std::vector <Observation> updatedCurve = meanCurve(clusterCurves);
        moved = this->getCentroid().getTimeSeries()->discreteFrechetDistance(updatedCurve);
        this->getCentroid().getTimeSeries()->setVector(updatedCurve);

        std::vector<double> vec;
        for (auto v: updatedCurve) {
            vec.push_back(v.y);
        }
        this->getCentroid().setVector(vec);
    }

    return moved;
}

void Cluster::print() {
    std::cout << "Centroid:" << std::endl;
    this->centroid.print();

    for (auto point: this->clusteredPoints) {
        std::cout << point->getId() << std::endl;
        std::cout << "  " << this->centroid.l2Distance(point) << std::endl;
    }
}