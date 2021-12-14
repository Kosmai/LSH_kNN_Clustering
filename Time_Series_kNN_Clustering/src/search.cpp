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
#include "../inc/readInput.hpp"

#define L2 0
#define DISC_FRECHET 1
#define CONT_FRECHET 2

int main(int argc, char **argv) {

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	setRandomSeed(seed);

	std::string inputFile  = "datasets/nasd_input.csv";
	std::string queryFile  = "datasets/our_query.csv";
	std::string outputFile = "out.txt";
	std::string algorithm = "Frechet";
	std::string metric = "discrete";

	int dims;
	int k = 4;
	int l = 5;
	int m = 50;
    int probes = 25;
	int numOfNearest = 10;
	double radius = 0.0;
	double delta = 1;
	double w;
	bool disableBruteForce = false;

	LSH* lsh = nullptr;
	Hypercube* cube = nullptr;

	std::vector<Point*> points;
	std::vector<TimeSeries*> timeSeries;

	if (readSearchArguments(argc, argv, inputFile, queryFile, k, l, m, probes, outputFile, algorithm, metric, delta, disableBruteForce) != 0) {
		std::cout << "Unknown argument read! Aborting..." << std::endl;
		return -1;
	}

	initializeInput(inputFile, dims, points);

	if(algorithm == "LSH"){
		lsh = initializeLSH(dims, l, k, w, points);
		printParameters(k, l, numOfNearest, radius, w);
		if(searchLoop(*lsh, queryFile, outputFile, numOfNearest, radius, L2, -1, -1, -1, disableBruteForce) < 0) return 2;
	}
	else if(algorithm == "Hypercube"){
		cube = initializeHypercube(dims, k, w, points);
		printParameters(k, l, numOfNearest, radius, w);
		if(searchLoop(*cube, queryFile, outputFile, m, probes, numOfNearest, radius) < 0) return 2;
	}
	else if(algorithm == "Frechet"){
		for(auto point : points){
			TimeSeries* t = new TimeSeries(point);
			timeSeries.push_back(t);
		}
		w = 200;
		double dx = delta;
		double dy = delta;
		double e = 0.1;
		printParameters(k, l, numOfNearest, radius, w);
		if(metric == "discrete"){
			lsh = initializeLSH(dims, l, k, w, timeSeries, dx, dy, -1);
			if(searchLoop(*lsh, queryFile, outputFile, numOfNearest, radius, DISC_FRECHET, dx, dy, -1, disableBruteForce) < 0) return 2;
		}
		else if(metric == "continuous"){
			lsh = initializeLSH(dims, l, k, w, timeSeries, -1, -1, e);
			if(searchLoop(*lsh, queryFile, outputFile, numOfNearest, radius, CONT_FRECHET, -1, -1, e, disableBruteForce) < 0) return 2;
		}
		else{
			std::cout << "Unknown metric entered" << std::endl;
			return 3;
		}
	}
	else{
		std::cout << "Unknown algorithm entered" << std::endl;
		return 4;
	}

	//free all memory
	if(algorithm == "Frechet"){
		for(auto point: lsh->getPoints()){
			delete point;
		}
	}
	for(auto ts: timeSeries){
		delete ts;
	}
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
