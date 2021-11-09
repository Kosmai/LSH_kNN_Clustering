#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <map>
#include "../inc/randGen.hpp"
#include "../inc/hashTable.hpp"
#include "../inc/point.hpp"
#include "../inc/readInput.hpp"
#include "../inc/hypercube.hpp"

#define W_MULTIPLIER 1

int main(int argc, char **argv) {

    std::string inputFile;
    std::string queryFile;
    int k = 0;
    int m = 0;
    int probes = 0;
    int dims;
    std::string outputFile;
    int numOfNearest = 0;
    double radius = 0.0;

    if (readHyperArguments(argc, argv, inputFile, queryFile, k, m, probes, outputFile, numOfNearest, radius) != 0) {
        std::cout << "Δεν εξυπηρετούμε ακόμα" << std::endl;
        return -1;
    }

    std::cout << "inputFile: " << inputFile << std::endl;
    std::cout << "queryFile: " << queryFile << std::endl;
    std::cout << "k: " << k << std::endl;
    std::cout << "M: " << m << std::endl;
    std::cout << "outputFile: " << outputFile << std::endl;
    std::cout << "numOfNearest: " << numOfNearest << std::endl;
    std::cout << "radius: " << radius << std::endl;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    setRandomSeed(seed);

    //queries is filled with all queries in their file
    std::vector <Point*> queries;
    std::vector <Point*> points;

    if (readDataSet(queryFile, ' ', queries) < 0) {
        std::cout << "Error while reading querry file. Aborting..." << std::endl;
        return 1;
    }
    if ((dims = readDataSet(inputFile, ' ', points)) < 0) {
       std::cout << "Error while reading input file. Aborting..." << std::endl;
       return 1;
    }

    double w = Hypercube::calculateW(points) * W_MULTIPLIER;
	std::cout << "w = " << w << std::endl;


    FILE* outfp = fopen(outputFile.c_str(),"w");

    Hypercube hyper = Hypercube(dims, pow(2, k), 1, k, w);
    for(auto point: points){
        hyper.addPoint(point);
    }

	for(unsigned int i = 0; i < queries.size(); i++){
        hyper.calculateNN(*queries[i], outfp, m, probes, numOfNearest, radius);
    }

	std::cout << hyper.averageRatio/queries.size() << std::endl;
	std::cout << hyper.worstDistance << std::endl;
	std::cout << hyper.distanceOver2 << std::endl;


	for(auto point: points){
		delete point;
	}
	for(auto point: queries){
		delete point;
	}
    fclose(outfp);
    return 0;
}
