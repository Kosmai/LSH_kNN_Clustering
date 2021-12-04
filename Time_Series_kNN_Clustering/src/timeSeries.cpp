#include <iostream>
#include "../inc/point.hpp"
#include "../inc/timeSeries.hpp"

TimeSeries::TimeSeries(Point* point) : id(point->getId()){
    int i = 0;
    for(auto coord : point->getVector()){
        Observation observation;
        observation.x = ++i;
        observation.y = coord;
        this->observations.push_back(observation);
    }
}

void TimeSeries::print(){
    for(auto obs : observations){
        std::cout << "(" << obs.x << ", " << obs.y << ")\n";
    }
}

Point* TimeSeries::snapToGrid(double dx, double dy){
    std::vector<double> elements;
    int x;
    double y;

    for(auto obs : this->observations){
        x = (int)((obs.x / dx) + 0.5);
        y = (int)((obs.y / dy) + 0.5);
        elements.push_back(x);
        elements.push_back(y);
    }

    Point* snapped = new Point(elements, this->id);

    snapped->print();
    return snapped;
}
