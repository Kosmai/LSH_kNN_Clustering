#include <iostream>
#include <list>
#include <string>
#include "readInput.hpp"
#include "point.hpp"

void findRandomCentroids(std::vector<Point*> points){
	int size = points.size();
	std::cout << size << std::endl;
}


int main(){
	std::vector<Point*> points;
	std::string inputFile = "datasets/input_small_id";
	readDataSet(inputFile, ' ', points);
	findRandomCentroids(points);
	return 0;
}