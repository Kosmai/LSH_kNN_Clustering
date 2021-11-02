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

#define MAX_ELEMENTS 10000
#define BUCKETS 1000 //make dynamic
#define DIMS 128
#define W 200

int readClArguments(int argc, char **argv, std::string &inputFile, std::string &queryFile, int &k, int &m, int &probes,
                    std::string &outputFile, int &numOfNearest, double &radius) {

    //keep track of what arguments have been read
    std::map<std::string, bool> argumentsRed;

    argumentsRed["-i"] = false;
    argumentsRed["-q"] = false;
    argumentsRed["-k"] = false;
    argumentsRed["-M"] = false;
    argumentsRed["-probes"] = false;
    argumentsRed["-o"] = false;
    argumentsRed["-N"] = false;
    argumentsRed["-R"] = false;

    //read config file first, then overwrite defaults by arguements if needed
    if (readHypercubeConfig("config/hypercube.conf", argumentsRed, inputFile, queryFile, k, m, probes,
                      outputFile, numOfNearest, radius) < 0) {
        std::cout << "Config file contains a malformed value." << std::endl;
    }

    //read arguments, overwrite anything needed
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
        } else if (std::string(argv[i]).compare("-M") == 0 && i + 1 < argc) {
            m = atoi(argv[i + 1]);
            argumentsRed["-M"] = true;
        } else if (std::string(argv[i]).compare("-probes") == 0 && i + 1 < argc) {
            probes = atoi(argv[i + 1]);
            argumentsRed["-probes"] = true;
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

    //missing arguments/config defaults
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
    int m = 0;
    int probes = 0;
    std::string outputFile;
    int numOfNearest = 0;
    double radius = 0.0;

    if (readClArguments(argc, argv, inputFile, queryFile, k, m, probes, outputFile, numOfNearest, radius) != 0) {
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
    std::vector <Point> queries;
    queryFile = "datasets/" + queryFile;
    if (readDataSet(queryFile, ' ', queries) < 0) {
        std::cout << "Error while reading querry file. Aborting..." << std::endl;
        return 1;
    }

    //Hypercube initialize will all points in inputfile
    inputFile = "datasets/" + inputFile;
    Hypercube hyper = Hypercube(DIMS, pow(2, k), 1, k, W);
    if (readDataSet(inputFile, ' ', hyper) < 0) {
       std::cout << "Error while reading input file. Aborting..." << std::endl;
       return 1;
    }
    //hyper.printAllHT();
    hyper.calculateNN(queries[0], m, probes, numOfNearest, radius);

    return 0;
}