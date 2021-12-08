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
    bool exists = false;

    for(auto obs : this->observations){
        x = (int)((obs.x / dx) + 0.5);
        y = (int)((obs.y / dy) + 0.5);

        for(int i = elements.size()-2; i > 0; i-=2){
            if(elements[i] == x){
                if(elements[i+1] == y){
                    exists = true;
                    break;
                }
            }
            else{
                break;
            }
        }
        if(!exists){
            elements.push_back(x);
            elements.push_back(y);
        }
        exists = false;
    }


    for(int i = elements.size(); i < (int)observations.size()*2; i+=2){
        elements.push_back(0);
        elements.push_back(0);
    }

    //std::cout << elements.size()/2 << std::endl;

    Point* snapped = new Point(elements, this->id);

    snapped->setTimeSeries(this);

    return snapped;
}

Point* TimeSeries::filter(double e, bool consecutiveErases){
    std::vector<double> filtered;

    for(auto observation: this->observations){
        filtered.push_back(observation.y);
    }

    //std::cout<<std::endl<<std::endl;

    double a, b, c;

    for(unsigned int i=0; i<filtered.size()-2; i++){

//        for(unsigned int i=0; i<filtered.size(); i++){
//            std::cout<<filtered[i]<<"->";
//        }
//        std::cout<<std::endl;

        a = filtered[i];
        b = filtered[i+1];
        c = filtered[i+2];

        //std::cout<< "a: "<< a <<" b: " << b << " c: " << c << std::endl;

        if((std::abs(a - b) < e) && (std::abs(b - c) < e)){
            //std::cout<<"deleted :" << filtered[i+1]<<std::endl;
            filtered.erase(filtered.begin() + i + 1);
            consecutiveErases ? i-- : i;
        }
    }

    for(unsigned int i = filtered.size(); i < observations.size(); i++){
        filtered.push_back(0);
    }

    /*
    for(unsigned int i=0; i<filtered.size(); i++){
        std::cout<<filtered[i]<<"->";
    }
    std::cout << std::endl;
    std::cout << filtered.size() << std::endl;
   
    std::cout<<std::endl;

    */
    Point* snapped = new Point(filtered, this->id);
    snapped->setTimeSeries(this);

    return snapped;
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

    std::vector<struct Leash> optimalTraversal;

    i = ts1_size-1;
    j = ts2_size-1;

    struct Leash leash;
    leash.i = i;
    leash.j = j;

    optimalTraversal.push_back(leash);

    while(i > 0 || j > 0){
        double min;
        if(i == 0){
            min = frechetArray[i][j-1];
        }
        
        else if(j == 0){
            min = frechetArray[i-1][j];
        }

        else{
            min = std::min(std::min(frechetArray[i-1][j], frechetArray[i][j-1]), frechetArray[i-1][j-1]);    
        }

        if(i > 0 && frechetArray[i-1][j] == min){
            i -= 1;
        }
        else if(j > 0 && frechetArray[i][j-1] == min){
            j -= 1;
        }
        else{
            i -= 1;
            j -= 1;
        }

        leash.i = i;
        leash.j = j;

        optimalTraversal.insert(optimalTraversal.begin(), leash);

    }

    std::vector<Observation> meanCurve;

    for(auto v: optimalTraversal){
        Observation observation;
        observation.x = (this->observations[v.i].x + otherObservations[v.j].x)/2;
        observation.y = (this->observations[v.i].y + otherObservations[v.j].y)/2;
        meanCurve.push_back(observation);
    }

    // std::cout<<"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<std::endl;
    // for(auto obs: this->observations){
    //     std:: cout << "(" << obs.x << ", " << obs.y << ") -> ";
    // }
    // std::cout<<"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<std::endl;
    // for(auto obs: otherObservations){
    //     std:: cout << "(" << obs.x << ", " << obs.y << ") -> ";
    // }
    // std::cout<<"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<std::endl;

    // for(auto obs: meanCurve){
    //     std:: cout << "(" << obs.x << ", " << obs.y << ") -> ";
    // }

    return frechetArray[ts1_size-1][ts2_size-1];
}

