#include <iostream>
#include <cmath>
#include "../inc/point.hpp"
#include "../inc/timeSeries.hpp"

TimeSeries::TimeSeries(Point* point){
    int i = 0;
    this->id = point->getId();
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

    snapped->setTimeSeries(this);

    return snapped;
}

Point* TimeSeries::filter(double e, bool consecutiveErases){
    std::vector<double> filtered;

    for(auto observation: this->observations){
        filtered.push_back(observation.y);
    }



    std::cout<<std::endl<<std::endl;

    double a, b, c;

    for(unsigned int i=0; i<filtered.size()-2; i++){

//        for(unsigned int i=0; i<filtered.size(); i++){
//            std::cout<<filtered[i]<<"->";
//        }
//        std::cout<<std::endl;

        a = filtered[i];
        b = filtered[i+1];
        c = filtered[i+2];

        std::cout<< "a: "<< a <<" b: " << b << " c: " << c << std::endl;

        if((std::abs(a - b) < e) && (std::abs(b - c) < e)){
            std::cout<<"deleted :" << filtered[i+1]<<std::endl;
            filtered.erase(filtered.begin() + i + 1);
            consecutiveErases ? i-- : i;
        }
    }

    for(unsigned int i=0; i<filtered.size(); i++){
        std::cout<<filtered[i]<<"->";
    }

    std::cout<<std::endl;

    return nullptr;
}

std::vector<Observation>& TimeSeries::getVector(){
    return this->observations;
}

double observationDistance(Observation& obs1, Observation& obs2){
    return sqrt(pow(obs1.x - obs2.x, 2) + pow(obs1.y - obs2.y, 2));
}

double TimeSeries::discreteFrechetDistance(TimeSeries* otherTs) {
    return discreteFrechetDistance(otherTs->getVector());
}

double TimeSeries::discreteFrechetDistance(std::vector <Observation> & otherObservations) {

    // amount of observations in each time series
    unsigned int ts1_size = this->observations.size();
    unsigned int ts2_size = otherObservations.size();

    if(ts1_size <= 0 || ts2_size <= 0){
        return -1;
    }

    // dynamic programming 2d array
    double** frechetArray = new double*[ts1_size];
    for (unsigned int i = 0; i < ts1_size; i++){
        frechetArray[i] = new double[ts2_size];
    }

    for (unsigned int i = 0; i < ts1_size; i++){
        for (unsigned int j = 0; j < ts2_size; j++){
            frechetArray[i][j] = -1;
        }
    }

    // fill
    frechetArray[0][0] = observationDistance(this->observations[0], otherObservations[0]);

    // fill first column
    for(unsigned int i = 1; i < ts1_size; i++) {
        frechetArray[i][0] = std::max(frechetArray[i - 1][0],
                                      observationDistance(this->observations[i], otherObservations[0]));
    }

    // fill first row
    for(unsigned int j = 1; j < ts2_size; j++){
        frechetArray[0][j] = std::max(frechetArray[0][j-1], observationDistance(this->observations[0], otherObservations[j]));
    }

    // fill rest positions
    unsigned int i, j;

    for (i = 1; i < ts1_size; i++){
        for(j = 1; j < ts2_size; j++){
            if(frechetArray[i][j] != -1){
                continue;
            }
            double minPrevDistance = std::min(std::min(frechetArray[i-1][j], frechetArray[i][j-1]), frechetArray[i-1][j-1]);
            frechetArray[i][j] = std::max(minPrevDistance, observationDistance(this->observations[i], otherObservations[j]));
        }
    }
/*
    for (i = 0; i < ts1_size; i++){
        for(j = 0; j < ts2_size; j++){

            std::cout<<frechetArray[i][j]<<" ";
        }

        std::cout<<std::endl<<std::endl;
    }

    std::cout<<frechetArray[ts1_size-1][ts2_size-1]<<std::endl;
*/
    return frechetArray[ts1_size-1][ts2_size-1];
}
