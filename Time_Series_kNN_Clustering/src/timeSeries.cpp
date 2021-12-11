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

int TimeSeries::setVector(std::vector<Observation> observations){
    this->observations = observations;
    return 0;
}

double observationDistance(Observation& obs1, Observation& obs2){
    return sqrt(pow(obs1.x - obs2.x, 2) + pow(obs1.y - obs2.y, 2));
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
        observation.x = (obs1[v.i].x + obs2[v.j].x)/2;
        observation.y = (obs1[v.i].y + obs2[v.j].y)/2;
        meanCurve.push_back(observation);
    }

    // std::cout<<"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<std::endl;
    // for(auto obs: obs1){
    //     std:: cout << "(" << obs.x << ", " << obs.y << ") -> ";
    // }
    // std::cout<<"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<std::endl;
    // for(auto obs: obs2){
    //     std:: cout << "(" << obs.x << ", " << obs.y << ") -> ";
    // }
    // std::cout<<"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<std::endl;

    // for(auto obs: meanCurve){
    //     std:: cout << "(" << obs.x << ", " << obs.y << ") -> ";
    // }


    //sampling 
    std::vector<Observation> sampledCurve;
    int sampleFrequency = std::ceil((double)meanCurve.size() / obs1.size());
    for(unsigned int i = 0; i < meanCurve.size(); i+=sampleFrequency){
        sampledCurve.push_back(meanCurve[i]);
    }

    for(unsigned int i = sampledCurve.size(); i < 120; i++){
        Observation ob;
        ob.x = i+1;
        ob.y = 0;
        sampledCurve.push_back(ob);
    }

    // for(auto obs: sampledCurve){
    //     std:: cout << "(" << obs.x << ", " << obs.y << ") -> ";
    // }

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