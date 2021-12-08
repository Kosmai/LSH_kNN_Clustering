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

#define L2 0
#define DISC_FRECHET 1
#define CONT_FRECHET 2

int main(int argc, char **argv) {

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	setRandomSeed(seed);

	std::string inputFile  = "datasets/nasdaq2017_LQ.csv";
	std::string queryFile  = "datasets/query.csv";
	std::string outputFile = "out.txt";

	int dims;
	int k = 4;
	int l = 5;
	int m = 1000;
    int probes = 500;
	int numOfNearest = 20;
	double radius = 0.0;
	double w;

	LSH* lsh = nullptr;
	Hypercube* cube = nullptr;
	LSH* lsh_ts = nullptr;

	std::vector<Point*> points;
	std::vector<TimeSeries*> timeSeries;

	initializeInput(inputFile, dims, points);

	// lsh = initializeLSH(dims, l, k, w, points);
	// cube = initializeHypercube(dims, k, w, points);

	// printParameters(k, l, numOfNearest, radius, w);

	// if(searchLoop(*lsh, queryFile, outputFile, numOfNearest, radius) < 0) return 2;
	// if(searchLoop(*cube, queryFile, outputFile, m, probes, numOfNearest, radius) < 0) return 2;

	for(auto point : points){
		TimeSeries* t = new TimeSeries(point);
		timeSeries.push_back(t);
	}
	w = 100;
	double dx = 1;
	double dy = 1;
	double e = -1;
	printParameters(k, l, numOfNearest, radius, w);
	lsh_ts = initializeLSH(dims, l, k, w, timeSeries, dx, dy, e);
	// //lsh_ts->printAllHT();

	if(searchLoop(*lsh_ts, queryFile, outputFile, numOfNearest, radius, DISC_FRECHET, dx, dy, e) < 0) return 2;

	//timeSeries[0]->snapToGrid(3,1)->print();
	//timeSeries[0]->filter(3);

	timeSeries[0]->discreteFrechetDistance(timeSeries[1]->getVector());

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
