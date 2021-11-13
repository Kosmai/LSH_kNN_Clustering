#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <map>
#include "../inc/hashFunctionG.hpp"
#include "../inc/randGen.hpp"
#include "../inc/hashTable.hpp"
#include "../inc/point.hpp"
#include "../inc/LSH.hpp"
#include "../inc/readInput.hpp"

#define W_MULTIPLIER 0.5
#define BUCKET_DIVISOR 4

int main(int argc, char **argv) {

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	setRandomSeed(seed);

	std::string inputFile  = "";
	std::string queryFile  = "";
	std::string outputFile = "";

	int dims;
	int k = 4;
	int l = 5;
	int numOfNearest = 1;
	double radius = 10000.0;

	std::vector<Point*> points;
	std::vector<Point*> queries;

	//read cli arguments
	if (readLshArguments(argc, argv, inputFile, queryFile, k, l, outputFile, numOfNearest, radius) != 0) {
		std::cout << "Unknown argument read! Aborting..." << std::endl;
		return -1;
	}
	
	if(inputFile == ""){
		std::cout << "Give path to input file: "; 
		std::cin >> inputFile;
	}
	
	if((dims = readDataSet(inputFile, ' ', points)) < 0){
		std::cout << "Error while reading input file. Aborting..." << std::endl;
		return 1;
	}

	//initialize buckets and w
	double w = LSH::calculateW(points)*W_MULTIPLIER;
	int buckets = points.size() >= BUCKET_DIVISOR ? points.size()/BUCKET_DIVISOR : 1;

	//LSH initialize will all points in inputfile
	LSH lsh = LSH(dims, buckets, l, k, w);
    for(auto point: points){
        lsh.addPoint(point);
    }

	//main program loop
	while(true){
		if(queryFile == ""){
			std::cout << "Give path to query file or \"exit\" to terminate: "; 
			std::cin >> queryFile;
			if(queryFile == "exit"){
				break;
			}
		}
		if(outputFile == ""){
			std::cout << "Give path to output file: "; 
			std::cin >> outputFile;
		}

		//prints all parameters
		std::cout << "--------------------------------------------------" << std::endl;
		std::cout << "Parameters" << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;
		std::cout << "k: " << k << std::endl;
		std::cout << "l: " << l << std::endl;
		std::cout << "N: " << numOfNearest << std::endl;
		std::cout << "R: " << radius << std::endl;
		std::cout << "w: " << w << std::endl;
		std::cout << "inputFile: " << inputFile << std::endl;
		std::cout << "queryFile: " << queryFile << std::endl;
		std::cout << "outputFile: " << outputFile << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;
		std::cout << "Results" << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;

		//read query file
		if(readDataSet(queryFile, ' ', queries) < 0){
			std::cout << "Error while reading query file. Aborting..." << std::endl;
			return 1;
		}

		//open output file
		FILE* outfp = fopen(outputFile.c_str(),"w");
		if(outfp == nullptr){
            std::cout << "Could not open output file. Aborting..." << std::endl;
            return 2;
        }

		//run the algorithm
		for(unsigned int i = 0; i < queries.size(); i++){
			lsh.calculateNN(*queries[i], outfp, numOfNearest, radius);
		}

		//statistics
		std::cout << "Statistics" << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;
		std::cout << "Average predicted/true distance ratio: " << lsh.averageRatio/lsh.successfulQueries << std::endl;
		std::cout << "Worst   predicted/true distance ratio: " << lsh.worstDistance << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;

		//cleanup - reset
		for(auto point: queries){
			delete point;
		}
		queries.clear();
		queryFile = "";
		outputFile = "";
		lsh.resetStatistics();
		fclose(outfp);
	}

	//free all memory and close all files
	for(auto point: points){
		delete point;
	}
	return 0;
}
