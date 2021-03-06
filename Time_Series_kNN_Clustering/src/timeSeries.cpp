#include <iostream>
#include <cmath>
#include "../inc/point.hpp"
#include "../inc/timeSeries.hpp"
#include "../fred/include/frechet.hpp"

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

Point* TimeSeries::snapToGrid(double dx, double dy, double tx, double ty){
    std::vector<double> elements;
    int x;
    double y;

    bool exists = false;

    // snapping
    for(auto obs : this->observations){
        x = (int)((obs.x / dx) + 0.5 + tx);
        y = (int)((obs.y / dy) + 0.5 + ty);

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

    // padding
    for(int i = elements.size(); i < (int)observations.size()*2; i+=2){
        elements.push_back(0);
        elements.push_back(0);
    }

    Point* snapped = new Point(elements, this->id);

    snapped->setTimeSeries(this);

    return snapped;
}

Point* TimeSeries::filter(double e, bool consecutiveErases, double delta, double t){
    std::vector<double> filtered;
    std::vector<double> snapped;

    for(auto observation: this->observations){
        filtered.push_back(observation.y);
    }


    double a, b, c;

    // parse the curve in triads checking if the
    // middle element should be removed
    for(unsigned int i=0; i<filtered.size()-2; i++){
        a = filtered[i];
        b = filtered[i+1];
        c = filtered[i+2];

        if((std::abs(a - b) < e) && (std::abs(b - c) < e)){

            filtered.erase(filtered.begin() + i + 1);
            consecutiveErases ? i-- : i;
        }
    }

    //snapping to R
    double x;
    for(auto item : filtered){
        x = (int)((item / delta) + 0.5 + t);
        snapped.push_back(x);
    }

    //padding
    for(unsigned int i = snapped.size(); i < observations.size(); i++){
        snapped.push_back(0);
    }




    Point* point = new Point(snapped, this->id);
    point->setTimeSeries(this);

    return point;
}

std::vector<Observation>& TimeSeries::getVector(){
    return this->observations;
}

int TimeSeries::setVector(std::vector<Observation> observations){
    this->observations = observations;
    return 0;
}

double observationDistance(Observation& obs1, Observation& obs2){
    return sqrt(pow(obs1.x - obs2.x, 2) + pow(obs1.y - obs2.y, 2));
}

double TimeSeries::continuousFrechetDistance(TimeSeries* otherTs, bool filterQueries){
    return continuousFrechetDistance(otherTs->getVector(), filterQueries);
}

double TimeSeries::continuousFrechetDistance(std::vector <Observation>& otherObservations, bool filterQueries, double e) {
    double distance;
    Curve ts1(2,"ts1");
    Curve ts2(2,"ts2");

    //create fred curves from time series
    if(filterQueries){
        for(auto observation: filterCurve(this->observations, e)){
            FredPoint p(2);
            p.set(0, observation.x);
            p.set(1, observation.y);
            ts1.push_back(p);
        }
        for(auto observation: filterCurve(otherObservations, e)){
            FredPoint p(2);
            p.set(0, observation.x);
            p.set(1, observation.y);
            ts2.push_back(p);
        }
    }
    else{
       for(auto observation: this->observations){
            FredPoint p(2);
            p.set(0, observation.x);
            p.set(1, observation.y);
            ts1.push_back(p);
        }
        for(auto observation: otherObservations){
            FredPoint p(2);
            p.set(0, observation.x);
            p.set(1, observation.y);
            ts2.push_back(p);
        } 
    }
    //calculate continuous frechet distance
    distance = Frechet::Continuous::distance(ts1, ts2).value;

    return distance;
}

double TimeSeries::discreteFrechetDistance(TimeSeries* otherTs) {
    return discreteFrechetDistance(otherTs->getVector());
}

double TimeSeries::discreteFrechetDistance(std::vector <Observation> & otherObservations) {
    double distance;
    meanCurve(this->observations, otherObservations, distance);
    return distance;
}

std::vector<Observation> meanCurve(std::vector<Observation> obs1, std::vector<Observation> obs2, double& frechetDistance){
  
    // amount of observations in each time series
    unsigned int ts1_size = obs1.size();
    unsigned int ts2_size = obs2.size();

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
    frechetArray[0][0] = observationDistance(obs1[0], obs2[0]);

    // fill first column
    for(unsigned int i = 1; i < ts1_size; i++) {
        frechetArray[i][0] = std::max(frechetArray[i - 1][0],
                                      observationDistance(obs1[i], obs2[0]));
    }

    // fill first row
    for(unsigned int j = 1; j < ts2_size; j++){
        frechetArray[0][j] = std::max(frechetArray[0][j-1], observationDistance(obs1[0], obs2[j]));
    }

    // fill rest positions
    unsigned int i, j;

    for (i = 1; i < ts1_size; i++){
        for(j = 1; j < ts2_size; j++){
            if(frechetArray[i][j] != -1){
                continue;
            }
            double minPrevDistance = std::min(std::min(frechetArray[i-1][j], frechetArray[i][j-1]), frechetArray[i-1][j-1]);
            frechetArray[i][j] = std::max(minPrevDistance, observationDistance(obs1[i], obs2[j]));
        }
    }

    std::vector<struct Leash> optimalTraversal;

    i = ts1_size-1;
    j = ts2_size-1;

    struct Leash leash;
    leash.i = i;
    leash.j = j;

    optimalTraversal.push_back(leash);

    // calculate optimal traversal
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

    // create mean curve
    for(auto v: optimalTraversal){
        Observation observation;
        observation.x = (obs1[v.i].x + obs2[v.j].x)/2;
        observation.y = (obs1[v.i].y + obs2[v.j].y)/2;
        meanCurve.push_back(observation);
    }

    //sampling 
    std::vector<Observation> sampledCurve;
    int sampleFrequency = std::ceil((double)meanCurve.size() / obs1.size());
    for(unsigned int i = 0; i < meanCurve.size(); i+=sampleFrequency){
        sampledCurve.push_back(meanCurve[i]);
    }

    for(unsigned int i = sampledCurve.size(); i < obs1.size(); i++){
        Observation ob;
        ob.x = i+1;
        ob.y = sampledCurve.back().y;
        sampledCurve.push_back(ob);
    }

    frechetDistance = frechetArray[ts1_size-1][ts2_size-1];

    for (unsigned int i = 0; i < ts1_size; i++){
        delete[] frechetArray[i];
    }
    delete[] frechetArray;

    return sampledCurve;  
}

std::vector<Observation> meanCurve(std::vector<std::vector<Observation>>& obs){
    std::vector<std::vector<Observation>> array = obs;
    unsigned int inc = 1;
    double dist;

    // pseudo map-reduce to find total mean curve
    while(inc < array.size()){
        for(unsigned int i = 0; i < array.size(); i+=(inc*2)){
            if(i+inc < array.size()){
                array[i] = meanCurve(array[i],array[i+inc], dist);
            }
        }
        inc*=2;
    }
    return array[0];
}


std::vector<Observation> filterCurve(std::vector<Observation>& obs, double e, bool consecutiveErases){
    std::vector<Observation> filtered;
    for(auto ob: obs){
        filtered.push_back(ob);
    }

    Observation a, b, c;

    for(unsigned int i=0; i<filtered.size()-2; i++){

        a = filtered[i];
        b = filtered[i+1];
        c = filtered[i+2];

        if((std::abs(a.y - b.y) < e) && (std::abs(b.y - c.y) < e)){

            filtered.erase(filtered.begin() + i + 1);
            consecutiveErases ? i-- : i;
        }
    }

    return filtered;
}
