#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <map>
#include "../inc/randGen.hpp"
#include "../inc/point.hpp"
#include "../inc/LSH.hpp"
#include "../inc/hypercube.hpp"
#include "../inc/searchUtils.hpp"
#include "../inc/timeSeries.hpp"

int main(int argc, char **argv) {

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	setRandomSeed(seed);

	std::string inputFile  = "datasets/nasdaq2017_LQ.csv";
	std::string queryFile  = "datasets/query.csv";
	std::string outputFile = "out.txt";

	int dims;
	// int k = 4;
	// int l = 5;
	// int m = 1000;
    // int probes = 500;
	// int numOfNearest = 5;
	// double radius = 0.0;
	// double w;

	LSH* lsh = nullptr;
	Hypercube* cube = nullptr;

	std::vector<Point*> points;

	initializeInput(inputFile, dims, points);

	// lsh = initializeLSH(dims, l, k, w, points);
	// cube = initializeHypercube(dims, k, w, points);

	// printParameters(k, l, numOfNearest, radius, w);

	// if(searchLoop(*lsh, queryFile, outputFile, numOfNearest, radius) < 0) return 2;
	// if(searchLoop(*cube, queryFile, outputFile, m, probes, numOfNearest, radius) < 0) return 2;

	TimeSeries t1(points[0]);
	t1.print();
	t1.snapToGrid(1,1);















	//free all memory
	for(auto point: points){
		delete point;
	}
	if(lsh != nullptr){
		delete lsh;
	}
	if(cube != nullptr){
		delete cube;
	}
	return 0;
}
