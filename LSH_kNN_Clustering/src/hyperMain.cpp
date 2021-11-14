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

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    setRandomSeed(seed);

    std::string inputFile  = "";
    std::string queryFile  = "";
    std::string outputFile = "";

    int dims;
    int k = 14;
    int m = 10;
    int probes = 2;
    int numOfNearest = 1;
    double radius = 10000.0;

    std::vector <Point*> queries;
    std::vector <Point*> points;

    //read cli arguments
    if (readHyperArguments(argc, argv, inputFile, queryFile, k, m, probes, outputFile, numOfNearest, radius) != 0) {
        std::cout << "Unknown argument read! Aborting..." << std::endl;
        return -1;
    }
    	
	if(inputFile == ""){
		std::cout << "Give path to input file: "; 
		std::cin >> inputFile;
	}
	
    std::cout << "Reading and analyzing input file...";
	fflush(stdout);
    if ((dims = readDataSet(inputFile, ' ', points)) < 0) {
       std::cout << "Error while reading input file. Aborting..." << std::endl;
       return 1;
    }
    std::cout << "done" << std::endl;

    //initialize w
    std::cout << "Calculating optimal w...";
    double w = Hypercube::calculateW(points) * W_MULTIPLIER;
    std::cout << "done" << std::endl;

    //Hypercube initialize will all points in inputfile
    std::cout << "Generating required structures...";
	fflush(stdout);
    Hypercube hyper = Hypercube(dims, pow(2, k), 1, k, w);
    for(auto point: points){
        hyper.addPoint(point);
    }
    std::cout << "done" << std::endl;

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

        //print all parameters
        std::cout << "--------------------------------------------------" << std::endl;
		std::cout << "Parameters" << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;
        std::cout << "inputFile: " << inputFile << std::endl;
        std::cout << "queryFile: " << queryFile << std::endl;
        std::cout << "k: " << k << std::endl;
        std::cout << "M: " << m << std::endl;
	    std::cout << "w = " << w << std::endl;
        std::cout << "outputFile: " << outputFile << std::endl;
        std::cout << "numOfNearest: " << numOfNearest << std::endl;
        std::cout << "radius: " << radius << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;
		std::cout << "Results" << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;

        //read query file
        if (readDataSet(queryFile, ' ', queries) < 0) {
            std::cout << "Error while reading querry file. Aborting..." << std::endl;
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
            hyper.calculateNN(*queries[i], outfp, m, probes, numOfNearest, radius);
        }

		//statistics
        if(hyper.successfulQueries > 0){
            std::cout << "Statistics" << std::endl;
            std::cout << "--------------------------------------------------" << std::endl;
            std::cout << "Average predicted/true distance ratio: " << hyper.averageRatio/hyper.successfulQueries << std::endl;
            std::cout << "Worst   predicted/true distance ratio: " << hyper.worstDistance << std::endl;
		    std::cout << "--------------------------------------------------" << std::endl;
        }

		//cleanup - reset
        for(auto point: queries){
            delete point;
        }
        queries.clear();
		queryFile = "";
		outputFile = "";
		hyper.resetStatistics();
        fclose(outfp);
    }
    for(auto point: points){
        delete point;
    }
    return 0;
}
