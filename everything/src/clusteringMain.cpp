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

#define MAX_ITERS 50
#define MAX_RADIUS 55000
#define MIN_TOLERACE 0.005
#define W_MULTIPLIER_LSH 1.5
#define W_MULTIPLIER_HYPER 0.5

int main(int argc, char** argv) {

    int clusters;
    int L,k,M,d, dims;
    int probes;

    std::string inputFile, configFile, outputFile, method;
    bool complete = false;

    std::vector<Point*> points;

    if(readClusterArguments(argc, argv, inputFile, configFile, complete,
                    outputFile, method) < 0){
        printf("Δεν εξυπηρετούμε ακόμα\n");
        return 1;
    }
    if(readClusterConfig(configFile, clusters, L, k, M, d, probes) < 0){
        printf("Error in config file, aborting...\n");
        return 1;
    }

    if((dims = readDataSet(inputFile, ' ', points)) < 0){
        printf("Error in reading points\n");
        return 1;
    }

    double w = LSH::calculateW(points);
	int buckets = points.size()/4;
	std::cout << "w = " << w << std::endl;

    FILE* outfp = fopen(outputFile.c_str(), "w");

    setRandomSeed(time(NULL));

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    Kmeans kmeans = Kmeans(dims, clusters);
    for(auto point: points){
        kmeans.addPoint(point);
    }

    auto t1 = high_resolution_clock::now();
    auto t2 = high_resolution_clock::now();

    if(method == "Classic"){
        fprintf(outfp, "Algorithm: Lloyds\n");
        t1 = high_resolution_clock::now();
        if(kmeans.computeLoyd(MIN_TOLERACE, MAX_ITERS, PlusPlus) < 0) return 3;
        t2 = high_resolution_clock::now();
    }
    else if(method == "LSH"){
        fprintf(outfp, "Algorithm: Range Search LSH\n");
        t1 = high_resolution_clock::now();
        if(kmeans.computeLSH(MAX_RADIUS, MAX_ITERS, PlusPlus, buckets, L, k, w*W_MULTIPLIER_LSH) < 0) return 3;
        t2 = high_resolution_clock::now();
    }
    else if(method == "Hypercube"){
        fprintf(outfp, "Algorithm: Range Search Hypercube\n");
        t1 = high_resolution_clock::now();
        if(kmeans.computeHypercube(MAX_RADIUS, MAX_ITERS, PlusPlus, d, w*W_MULTIPLIER_HYPER, probes, M) < 0) return 3;
        t2 = high_resolution_clock::now();
    }
    else{
        printf("Unknown method. Try \"Classic\" or \"LSH\" or \"Hypercube\" \n");
        return 2;
    }

    kmeans.printClusters(outfp);

    auto time = duration_cast<milliseconds>(t2 - t1);
    fprintf(outfp, "Clustering_time: %.3lf\n", (double)time.count()/1000);

    kmeans.displaySilhouette(outfp);

    if(complete){
        kmeans.printCompleteInfo(outfp);
    }


    fclose(outfp);


    for(auto point: points){
        delete point;
    }

    return 0;
}