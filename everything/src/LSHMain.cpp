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

int main(int argc, char **argv) {

	std::string inputFile;
	std::string queryFile;
	int k = 0;
	int l = 0;
	int dims;
	std::string outputFile;
	int numOfNearest = 0;
	double radius = 0.0;

	if (readLshArguments(argc, argv, inputFile, queryFile, k, l, outputFile, numOfNearest, radius) != 0) {
		std::cout << "Δεν εξυπηρετούμε ακόμα" << std::endl;
		return -1;
	}

	std::cout << "inputFile: " << inputFile << std::endl;
	std::cout << "queryFile: " << queryFile << std::endl;
	std::cout << "k: " << k << std::endl;
	std::cout << "l: " << l << std::endl;
	std::cout << "outputFile: " << outputFile << std::endl;
	std::cout << "numOfNearest: " << numOfNearest << std::endl;
	std::cout << "radius: " << radius << std::endl;

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	setRandomSeed(seed);

	//queries is filled with all queries in their file
	std::vector<Point*> queries;
	std::vector<Point*> points;

	if(readDataSet(queryFile, ' ', queries) < 0){
		std::cout << "Error while reading querry file. Aborting..." << std::endl;
		return 1;
	}
	if((dims = readDataSet(inputFile, ' ', points)) < 0){
		std::cout << "Error while reading input file. Aborting..." << std::endl;
		return 1;
	}

	double w = LSH::calculateW(points)*W_MULTIPLIER;
	int buckets = points.size()/4;
	std::cout << "w = " << w << std::endl;

	FILE* outfp = fopen(outputFile.c_str(),"w");

	//LSH initialize will all points in inputfile
	LSH lsh = LSH(dims, buckets, l, k, w);

    for(auto point: points){
        lsh.addPoint(point);
    }

	for(unsigned int i = 0; i < queries.size(); i++){
		lsh.calculateNN(*queries[i], outfp, numOfNearest, radius);
	}

	std::cout << lsh.averageRatio/queries.size() << std::endl;
	std::cout << lsh.worstDistance << std::endl;

	for(auto point: points){
		delete point;
	}
	for(auto point: queries){
		delete point;
	}
	fclose(outfp);
	return 0;
}
