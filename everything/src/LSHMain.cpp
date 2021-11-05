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

#define MAX_ELEMENTS 10000
#define BUCKETS 1000 //make dynamic
#define DIMS 128
#define W 200

int main(int argc, char **argv) {

	std::string inputFile;
	std::string queryFile;
	int k = 0;
	int l = 0;
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
	std::vector<Point> queries;
	queryFile = "datasets/" + queryFile;
	if(readDataSet(queryFile, ' ', queries) < 0){
		std::cout << "Error while reading querry file. Aborting..." << std::endl;
		return 1;
	}

	//LSH initialize will all points in inputfile
	inputFile = "datasets/" + inputFile;
	LSH lsh = LSH(128, BUCKETS, l, k, W);
	if(readDataSet(inputFile, ' ', lsh) < 0){
		std::cout << "Error while reading input file. Aborting..." << std::endl;
		return 1;
	}
	//lsh.printAllHT();
	lsh.calculateNN(queries[0], 10, 350);

	return 0;
}
