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
#include "../inc/hypercube.hpp"
#include "../inc/readInput.hpp"
#include "../inc/searchUtils.hpp"
#include "../inc/timeSeries.hpp"

#define W_LSH_MULTIPLIER 0.1
#define W_CUBE_MULTIPLIER 0.2
#define BUCKET_DIVISOR 4


Hypercube* initializeHypercube(int dims, int k, double &w, std::vector<Point*> &points){
	//initialize w
    std::cout << "Calculating optimal w...";
    w = Hypercube::calculateW(points) * W_CUBE_MULTIPLIER;
    std::cout << "done" << std::endl;

    //Hypercube initialize will all points in inputfile
    std::cout << "Generating required structures...";
	fflush(stdout);
    Hypercube* cube = new Hypercube(dims, pow(2, k), 1, k, w);
    for(auto point: points){
        cube->addPoint(point);
    }
    std::cout << "done" << std::endl;
	return cube;
}

LSH* initializeLSH(int dims, int l, int k, double &w, std::vector<Point*> &points){
	//initialize buckets and w
	std::cout << "Calculating optimal w...";
	fflush(stdout);
	w = LSH::calculateW(points)*W_LSH_MULTIPLIER;
	int buckets = points.size() >= BUCKET_DIVISOR ? points.size()/BUCKET_DIVISOR : 1;
	std::cout << "done" << std::endl;

	//LSH initialize will all points in inputfile
	std::cout << "Generating required structures...";
	fflush(stdout);
	LSH* lsh = new LSH(dims, buckets, l, k, w);
    for(auto point: points){
        lsh->addPoint(point);
    }
	std::cout << "done" << std::endl;
	return lsh;
}

LSH* initializeLSH(int dims, int l, int k, double &w, std::vector<TimeSeries*> &ts, double dx, double dy, double filter_e){
	//initialize buckets and w
	std::cout << "Calculating optimal w...";
	fflush(stdout);
	//w = LSH::calculateW(points)*W_LSH_MULTIPLIER;
	int buckets = ts.size() >= BUCKET_DIVISOR ? ts.size()/BUCKET_DIVISOR : 1;
	std::cout << "done" << std::endl;

	//LSH initialize will all timeSeries in inputfile
	std::cout << "Generating required structures...";
	LSH* lsh;
	fflush(stdout);
	if(filter_e < 0){
		lsh = new LSH(2*dims, buckets, l, k, w);
		for(auto t: ts){
			lsh->addTimeSeries(t, dx, dy);
		}
	}
	else{
		lsh = new LSH(dims, buckets, l, k, w);
		for(auto t: ts){
			lsh->addTimeSeries(t, dx, dy, filter_e);
		}
	}

	std::cout << "done" << std::endl;
	return lsh;
}

int initializeInput(std::string &inputFile, int &dims, std::vector<Point*> &points){

	//read cli arguments
	// if (readLshArguments(argc, argv, inputFile, queryFile, k, l, outputFile, numOfNearest, radius) != 0) {
	// 	std::cout << "Unknown argument read! Aborting..." << std::endl;
	// 	return -1;
	// }
	
	if(inputFile == ""){
		std::cout << "Give path to input file: "; 
		std::cin >> inputFile;
	}
	
	std::cout << "Reading and analyzing input file...";
	fflush(stdout);
	if((dims = readDataSet(inputFile, '\t', points)) < 0){
		std::cout << "Error while reading input file. Aborting..." << std::endl;
		return -1;
	}
	std::cout << "done" << std::endl;
	return 0;
}

void printParameters(int k, int l, int numOfNearest, double radius, int w){
	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "Parameters" << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "k: " << k << std::endl;
	std::cout << "l: " << l << std::endl;
	std::cout << "N: " << numOfNearest << std::endl;
	std::cout << "R: " << radius << std::endl;
	std::cout << "w: " << w << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;
	return;
}

int searchLoop(LSH& lsh, std::string queryFile, std::string outputFile, int numOfNearest, double radius, int metric, double dx, double dy, double filter_e){

	std::vector<Point*> queries;

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
		std::cout << "Files" << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;
		std::cout << "queryFile: " << queryFile << std::endl;
		std::cout << "outputFile: " << outputFile << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;
		std::cout << "Results" << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;

		//read query file
		if(readDataSet(queryFile, '\t', queries) < 0){
			std::cout << "Error while reading query file. Aborting..." << std::endl;
			return -1;
		}

		//open output file
		FILE* outfp = fopen(outputFile.c_str(),"w");
		if(outfp == nullptr){
            std::cout << "Could not open output file. Aborting..." << std::endl;
            return -2;
        }

		//run the algorithm
		for(unsigned int i = 0; i < queries.size(); i++){
			if(metric == 0){
				lsh.calculateNN(*queries[i], outfp, numOfNearest, radius, metric);
			}
			if(metric == 1){
				TimeSeries t1(queries[i]);
				Point* p = t1.snapToGrid(dx, dy);
				lsh.calculateNN(*p, outfp, numOfNearest, radius, metric);
			}
			if(metric == 2){
				TimeSeries t1(queries[i]);
				Point* p = t1.filter(filter_e);
				lsh.calculateNN(*p, outfp, numOfNearest, radius, metric);
			}
		}

		//statistics
		if(lsh.successfulQueries > 0){
			std::cout << "Statistics" << std::endl;
			std::cout << "--------------------------------------------------" << std::endl;
			std::cout << "Average predicted/true distance ratio: " << lsh.averageRatio/lsh.successfulQueries << std::endl;
			std::cout << "Worst   predicted/true distance ratio: " << lsh.worstDistance << std::endl;
			std::cout << "--------------------------------------------------" << std::endl;
		}
		
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
	return 0;
}

int searchLoop(Hypercube& cube, std::string queryFile, std::string outputFile, int probes, int m, int numOfNearest, double radius){

	std::vector<Point*> queries;

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
		std::cout << "Files" << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;
		std::cout << "queryFile: " << queryFile << std::endl;
		std::cout << "outputFile: " << outputFile << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;
		std::cout << "Results" << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;

		//read query file
		if(readDataSet(queryFile, '\t', queries) < 0){
			std::cout << "Error while reading query file. Aborting..." << std::endl;
			return -1;
		}

		//open output file
		FILE* outfp = fopen(outputFile.c_str(),"w");
		if(outfp == nullptr){
            std::cout << "Could not open output file. Aborting..." << std::endl;
            return -2;
        }

		//run the algorithm
		for(unsigned int i = 0; i < queries.size(); i++){
			cube.calculateNN(*queries[i], outfp, m, probes, numOfNearest, radius);
		}

		//statistics
		if(cube.successfulQueries > 0){
			std::cout << "Statistics" << std::endl;
			std::cout << "--------------------------------------------------" << std::endl;
			std::cout << "Average predicted/true distance ratio: " << cube.averageRatio/cube.successfulQueries << std::endl;
			std::cout << "Worst   predicted/true distance ratio: " << cube.worstDistance << std::endl;
			std::cout << "--------------------------------------------------" << std::endl;
		}
		
		//cleanup - reset
		for(auto point: queries){
			delete point;
		}
		queries.clear();
		queryFile = "";
		outputFile = "";
		cube.resetStatistics();
		fclose(outfp);
	}
	return 0;
}
