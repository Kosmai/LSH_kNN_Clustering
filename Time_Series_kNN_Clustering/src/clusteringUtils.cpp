#include <iostream>
#include <string>

#include "../inc/clusteringUtils.hpp"

int getMetric(std::string update){
    int metric;
    if(update == "MeanVector"){
        metric = 0;
    }
    else if(update == "MeanFrechet"){
        metric = 1;
    }
    else{
        std::cout << "Unknown update method. <Mean Vector> or <Mean Frechet> accepted." << std::endl;
        return -1;
    }
    return metric;
}

void displayClusterParameters(std::string &inputFile, std::string &configFile, std::string &outputFile, 
                         std::string &update, std::string &assignment, bool &complete, bool &silhouette,
                         int k, int L, int M, int d, int w, int probes, int clusters){

    //print all parameters
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "Parameters" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "inputFile: " << inputFile << std::endl;
    std::cout << "configFile: " << configFile << std::endl;
    std::cout << "outputFile: " << outputFile << std::endl;
    std::cout << "assignment: " << assignment << std::endl;
    std::cout << "update: " << update << std::endl;
    std::cout << "base w: " << w << std::endl;
    std::cout << "probes: " << probes << std::endl;
    std::cout << "clusters: " << clusters << std::endl;
    std::cout << "k: " << k << std::endl;
    std::cout << "L: " << L << std::endl;
    std::cout << "M: " << M << std::endl;
    std::cout << "d: " << d << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "Results" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    return;
}