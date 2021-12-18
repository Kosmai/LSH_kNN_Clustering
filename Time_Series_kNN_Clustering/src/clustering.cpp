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
#include "../inc/timeSeries.hpp"
#include "../inc/clusteringUtils.hpp"

#define MAX_ITERS 5
#define MAX_RADIUS 55000
#define MIN_TOLERACE 0.05
#define W_MULTIPLIER_LSH 1.5
#define W_MULTIPLIER_HYPER 0.5
#define BUCKET_DIVISOR 4

int main(int argc, char** argv) {

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	setRandomSeed(seed);

    std::string inputFile  = "";
	std::string configFile  = "";
	std::string outputFile = "";

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();
    auto t2 = high_resolution_clock::now();

    bool complete = false;
    bool silhouette = false;
    int clusters = 2;
    int probes = 2;
    int L = 3;
    int k = 4;
    int M = 10;
    int d = 3;
    int dims;

    int metric = 1;

    std::string assignment = "Classic";
    std::string update = "MeanFrechet";

    std::vector<Point*> points;
    std::vector<TimeSeries*> timeSeries;

    //read cli arguments
    if(readClusterArguments(argc, argv, inputFile, configFile, outputFile,
                    update, assignment, complete, silhouette) < 0){
        std::cout << "Error in reading arguments! Aborting..." << std::endl;
        return 1;
    }
    //read config file and overwrite any default value needed
    if(readClusterConfig(configFile, clusters, L, k, M, d, probes) < 0){
        std::cout << "Error in config file! Aborting..." << std::endl;
        return 1;
    }
    //read all points from inputFile
    if((dims = readDataSet(inputFile, '\t', points)) < 0){
        std::cout << "Error in reading points. Aborting..." << std::endl;
        return 1;
    }

    //find metric used based on update entered
    metric = getMetric(update);

    //assign time series
    for(auto point : points){
		TimeSeries* t = new TimeSeries(point);
        point->setTimeSeries(t);
		timeSeries.push_back(t);
	}

	//initialize buckets and w
	double w = LSH::calculateW(points);
	int buckets = points.size() >= BUCKET_DIVISOR ? points.size()/BUCKET_DIVISOR : 1;

    //print all parameters
    displayClusterParameters(inputFile,configFile,outputFile,update,assignment,complete,silhouette,k,L,M,d,w,probes,clusters);

    //open output file
    FILE* outfp = fopen(outputFile.c_str(), "w");
    if(outfp == nullptr){
        std::cout << "Could not open output file. Aborting..." << std::endl;
        return 2;
    }

    //initialize kmeans structure and add all points
    Kmeans kmeans = Kmeans(dims, clusters, metric);
    for(auto point: points){
        kmeans.addPoint(point);
    }

    //select the assignment given by the user
    if(assignment == "Classic"){
        (metric == 0) ? fprintf(outfp, "A Lloyds U Vector\n") : fprintf(outfp, "A Lloyds U Frechet\n");;
        t1 = high_resolution_clock::now();
        if(kmeans.computeLoyd(MIN_TOLERACE, MAX_ITERS, PlusPlus, metric) < 0) return 3;
        t2 = high_resolution_clock::now();
    }
    else if(assignment == "LSH"){
        fprintf(outfp, "A LSH U Vector\n");
        if(metric != 0){
            std::cout << "Unsupported update method for LSH. <Mean Vector> only." << std::endl;
            return 4;
        }
        t1 = high_resolution_clock::now();
        if(kmeans.computeLSH(MAX_RADIUS, MAX_ITERS, PlusPlus, buckets, L, k, w*W_MULTIPLIER_LSH, metric) < 0) return 3;
        t2 = high_resolution_clock::now();
    }
    else if(assignment == "Hypercube"){
        fprintf(outfp, "A Hypercube U Vector\n");
        if(metric != 0){
            std::cout << "Unsupported update method for hypercube. <Mean Vector> only." << std::endl;
            return 4;
        }
        t1 = high_resolution_clock::now();
        if(kmeans.computeHypercube(MAX_RADIUS, MAX_ITERS, PlusPlus, d, w*W_MULTIPLIER_HYPER, probes, M) < 0) return 3;
        t2 = high_resolution_clock::now();
    }
    else if(assignment == "LSH_Frechet"){
        fprintf(outfp, "A LSH U Frechet\n");
        if(metric != 1){
            std::cout << "Unsupported update method for LSH_Frechet. <Mean Frechet> only." << std::endl;
            return 4;
        }
        t1 = high_resolution_clock::now();
        if(kmeans.computeLSH(MAX_RADIUS, MAX_ITERS, PlusPlus, buckets, L, k, w*W_MULTIPLIER_LSH, metric) < 0) return 3;
        t2 = high_resolution_clock::now();
    }
    else{
        printf("Unknown assignment. Try \"Classic\" or \"LSH\" or \"Hypercube\" or \"LSH_Frechet\"\n");
        return 2;
    }

    kmeans.printClusters(outfp);

    auto time = duration_cast<milliseconds>(t2 - t1);
    fprintf(outfp, "Clustering_time: %.3lf\n", (double)time.count()/1000);



    if(silhouette){
        //close file (avoid waiting for silhouette calculation to see already printed results)
        fclose(outfp);
        //open file again to print silhouette and complete info
        outfp = fopen(outputFile.c_str(), "a");
        if(outfp == nullptr){
            std::cout << "Could not open output file. Aborting..." << std::endl;
            return 2;
        }
        kmeans.displaySilhouette(outfp, metric);
    }

    if(complete){
        kmeans.printCompleteInfo(outfp);
    }

    //free all memory
    for(auto ts: timeSeries){
        delete ts;
    }
    for(auto point: points){
        delete point;
    }
    fclose(outfp);
    return 0;
}