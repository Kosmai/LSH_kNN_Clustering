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
#define MIN_TOLERACE 0.05
#define W_MULTIPLIER_LSH 1.5
#define W_MULTIPLIER_HYPER 0.5
#define BUCKET_DIVISOR 4

int main(int argc, char** argv) {

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	setRandomSeed(seed);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();
    auto t2 = high_resolution_clock::now();

    bool complete = false;
    int clusters = 2;
    int probes = 2;
    int L = 3;
    int k = 4;
    int M = 10;
    int d = 3;
    int dims;

    std::string inputFile, configFile, outputFile, method;

    std::vector<Point*> points;

    //read cli arguments
    if(readClusterArguments(argc, argv, inputFile, configFile, complete,
                    outputFile, method) < 0){
        std::cout << "Error in reading arguments! Aborting..." << std::endl;
        return 1;
    }
    //read config file and overwrite any default value needed
    if(readClusterConfig(configFile, clusters, L, k, M, d, probes) < 0){
        std::cout << "Error in config file! Aborting..." << std::endl;
        return 1;
    }
    //read all points from inputFile
    if((dims = readDataSet(inputFile, ' ', points)) < 0){
        std::cout << "Error in reading points. Aborting..." << std::endl;
        return 1;
    }

	//initialize buckets and w
	double w = LSH::calculateW(points);
	int buckets = points.size() >= BUCKET_DIVISOR ? points.size()/BUCKET_DIVISOR : 1;

    //print all parameters
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "Parameters" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "k: " << k << std::endl;
    std::cout << "L: " << L << std::endl;
    std::cout << "M: " << M << std::endl;
    std::cout << "d: " << d << std::endl;
    std::cout << "base w: " << w << std::endl;
    std::cout << "probes: " << probes << std::endl;
    std::cout << "clusters: " << clusters << std::endl;
    std::cout << "inputFile: " << inputFile << std::endl;
    std::cout << "outputFile: " << outputFile << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "Results" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    //open output file
    FILE* outfp = fopen(outputFile.c_str(), "w");
    if(outfp == nullptr){
        std::cout << "Could not open output file. Aborting..." << std::endl;
        return 2;
    }

    //initialize kmeans structure and add all points
    Kmeans kmeans = Kmeans(dims, clusters);
    for(auto point: points){
        kmeans.addPoint(point);
    }

    //select the method given by the user
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

    //if the complete argument was given, print additional info
    if(complete){
        kmeans.printCompleteInfo(outfp);
    }

    for(auto point: points){
        delete point;
    }
    fclose(outfp);
    return 0;
}