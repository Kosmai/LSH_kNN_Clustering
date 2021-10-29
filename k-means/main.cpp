#include <iostream>
#include <list>
#include <string>
#include "readInput.hpp"
#include "point.hpp"


int main(){
	std::vector<Point*> points;
	std::string inputFile = "datasets/input_small_id";
	readDataSet(inputFile, ' ', points);
	return 0;
}