#include <iostream>
#include <list>
#include <string>
#include <algorithm>
#include <random>
#include "readInput.hpp"
#include "point.hpp"
#include "randGen.hpp"

void findRandomCentroids(std::vector<Point*> points, int k, int* centroids){
	int size = points.size();
	std::cout << size << std::endl;
	std::vector<Point*> shuffledPoints = points;
	std::random_shuffle(std::begin(shuffledPoints), std::end(shuffledPoints));
	for(int i = 0; i < k; i++){
		centroids[i] = stoi(shuffledPoints[i]->getId());
	}
}


int main(){
	int k = 100;
	setRandomSeed(time(NULL));
	std::vector<Point*> points;
	std::string inputFile = "datasets/input_small_id";
	readDataSet(inputFile, ' ', points);

	int* centroids = new int[k];

	findRandomCentroids(points, k, centroids);

	for(int i = 0; i < k; i++){
		std::cout << centroids[i] << std::endl;
	}
	return 0;
}