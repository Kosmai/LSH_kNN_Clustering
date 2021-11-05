#include <iostream>
#include <list>
#include <string>
#include <algorithm>
#include <random>
#include <cfloat>
#include <chrono>
#include "../inc/readInput.hpp"
#include "../inc/point.hpp"
#include "../inc/randGen.hpp"
#include "../inc/cluster.hpp"
#include "../inc/kmeans.hpp"
#include "../inc/LSH.hpp"

#define DIMS 128
#define CLUSTERS 3
#define MAX_ITERS 50
#define MAX_RADIUS 55000
#define MIN_TOLERACE 0.005

int main(int argc, char** argv) {

    int clusters;
    int L,k,M,d;
    int probes;
    int buckets = 1000;
    int w = 200;

    std::string inputFile, configFile, outputFile, method;
    bool complete = false;
    if(readClusterArguments(argc, argv, inputFile, configFile, complete,
                    outputFile, method) < 0){
        printf("Δεν εξυπηρετούμε ακόμα\n");
        return 1;
    }
    if(readClusterConfig(configFile, clusters, L, k, M, d, probes) < 0){
        printf("Error in config file, aborting...\n");
        return 1;
    }


    setRandomSeed(time(NULL));

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    Kmeans kmeans = Kmeans(DIMS, clusters);

    readDataSet(inputFile, ' ', kmeans);

    auto t1 = high_resolution_clock::now();
    auto t2 = high_resolution_clock::now();

    if(method == "Classic"){
        t1 = high_resolution_clock::now();
        kmeans.computeLoyd(MIN_TOLERACE, MAX_ITERS, Random);
        t2 = high_resolution_clock::now();
    }
    else if(method == "LSH"){
        t1 = high_resolution_clock::now();
        kmeans.computeLSH(MAX_RADIUS, MAX_ITERS, Random, buckets, L, k, w);
        t2 = high_resolution_clock::now();
    }
    else if(method == "Hypercube"){
        t1 = high_resolution_clock::now();
        kmeans.computeHypercube(MAX_RADIUS, MAX_ITERS, Random, d, w, probes, M);
        t2 = high_resolution_clock::now();
    }
    else{
        printf("Unknown method. Try \"Classic\" or \"LSH\" or \"Hypercube\" \n");
        return 2;
    }
    auto time = duration_cast<milliseconds>(t2 - t1);
    std::cout << "Total time in ms: " << time.count() << std::endl;

    //kmeans.displaySilhouette();
    return 0;
}