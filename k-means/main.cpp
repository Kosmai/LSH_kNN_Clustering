#include <iostream>
#include <list>
#include <string>
#include <algorithm>
#include <random>
#include "readInput.hpp"
#include "point.hpp"
#include "randGen.hpp"
#include "cluster.hpp"

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
//	int k = 100;
//	setRandomSeed(time(NULL));
//	std::vector<Point*> points;
//	std::string inputFile = "datasets/input_small_id";
//	readDataSet(inputFile, ' ', points);
//
//	int* centroids = new int[k];
//
//	findRandomCentroids(points, k, centroids);
//
//	for(int i = 0; i < k; i++){
//		std::cout << centroids[i] << std::endl;
//	}

    Cluster a(128);
    std::vector<double> vec1(128, 1);
    std::vector<double> vec2(128, 2);
    std::vector<double> vec3(128, 3);

    Point* p1 = new Point(vec1, "test_point_1");
    Point* p2 = new Point(vec2, "test_point_2");
    Point* p3 = new Point(vec3, "test_point_3");

    a.setCentroid(p1);
    a.insertPoint(p2);
    a.insertPoint(p3);

    a.recenter();
    a.print();
    return 0;
}