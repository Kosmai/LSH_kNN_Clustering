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

#define MAX_ELEMENTS 10000
#define BUCKETS 100
#define DIMS 2
#define W 2
#define K 4
#define L 8

int readClArguments(int argc, char **argv, std::string &inputFile, std::string &queryFile, int &k, int &l,
                    std::string &outputFile, int &numOfNearest, double &radius) {

    //keep track of what arguments have been red
    std::map<std::string, bool> argumentsRed;

    argumentsRed["-i"] = false;
    argumentsRed["-q"] = false;
    argumentsRed["-k"] = false;
    argumentsRed["-L"] = false;
    argumentsRed["-o"] = false;
    argumentsRed["-N"] = false;
    argumentsRed["-R"] = false;

    //read arguments
    for (int i = 1; i < argc; i += 2) {
        if (std::string(argv[i]).compare("-i") == 0 && i + 1 < argc) {
            inputFile = argv[i + 1];
            argumentsRed["-i"] = true;
        } else if (std::string(argv[i]).compare("-q") == 0 && i + 1 < argc) {
            queryFile = argv[i + 1];
            argumentsRed["-q"] = true;
        } else if (std::string(argv[i]).compare("-k") == 0 && i + 1 < argc) {
            k = atoi(argv[i + 1]);
            argumentsRed["-k"] = true;
        } else if (std::string(argv[i]).compare("-L") == 0 && i + 1 < argc) {
            l = atoi(argv[i + 1]);
            argumentsRed["-L"] = true;
        } else if (std::string(argv[i]).compare("-o") == 0 && i + 1 < argc) {
            outputFile = argv[i + 1];
            argumentsRed["-o"] = true;
        } else if (std::string(argv[i]).compare("-N") == 0 && i + 1 < argc) {
            numOfNearest = atoi(argv[i + 1]);
            argumentsRed["-N"] = true;
        } else if (std::string(argv[i]).compare("-R") == 0 && i + 1 < argc) {
            radius = atof(argv[i + 1]);
            argumentsRed["-R"] = true;
        } else {            //unknown argument
            return -1;
        }
    }

    //missing arguments
    for (std::pair<std::string, bool> arg: argumentsRed) {
        if (!arg.second) {
            return -2;
        }
    }

    return 0;
}

int main(int argc, char **argv) {

    std::string inputFile;
    std::string queryFile;
    int k = 0;
    int l = 0;
    std::string outputFile;
    int numOfNearest = 0;
    double radius = 0.0;

    if (readClArguments(argc, argv, inputFile, queryFile, k, l, outputFile, numOfNearest, radius) != 0) {
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

//    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//    setRandomSeed(seed);
//
//    std::vector<Point> points;
//	std::vector<double> p[MAX_ELEMENTS];
//	for(int i = 0; i < MAX_ELEMENTS; i++){
//		for(int j = 0; j < DIMS; j++){
//			p[i].push_back(getNormalRandom()*10);
//		}
//        points.push_back(Point(p[i], "temp"));
//	}
//
//    LSH lsh = LSH(DIMS, BUCKETS, L, K, W);
//    for(int i = 0; i < MAX_ELEMENTS; i++){
//        lsh.addPoint(points[i]);
//    }
//    lsh.printAllHT();

    return 0;
}