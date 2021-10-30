#include <iostream>
#include <list>
#include <string>
#include <algorithm>
#include <random>
#include <cfloat>
#include "readInput.hpp"
#include "point.hpp"
#include "randGen.hpp"
#include "cluster.hpp"

#define DIMS 128

void findRandomCentroids(std::vector<Point*> points, int k, int* centroids){
	//int size = points.size();
	//std::cout << size << std::endl;
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
	std::vector<Point*> queries;
    std::string inputFile = "datasets/input_small_id";
	std::string queryFile = "datasets/query_small_id";
    readDataSet(inputFile, ' ', points);
	readDataSet(queryFile, ' ', queries);

	int* centroids = new int[k];

	findRandomCentroids(points, k, centroids);

	// for(int i = 0; i < k; i++){
	// 	std::cout << centroids[i] << std::endl;
	// }

    Cluster* clusters = new Cluster[k];
    for(int i = 0; i < k; i++){
        clusters[i] = Cluster(DIMS);
        clusters[i].setCentroid(points[centroids[i]]);
        //clusters[i].setCentroid(queries[i]);
    }
    double dist, minDist;
    int minIndex;



    for(int iter = 0; iter < 10; iter++){
        for(auto point : points){
            minDist = DBL_MAX;
            minIndex = -1;
            for(int i = 0; i < k; i++){
                dist = clusters[i].getCentroid().l2Distance(point);
                if(dist <= minDist){
                    minDist = dist;
                    minIndex = i;
                }
            }
            clusters[minIndex].insertPoint(point);
        }
        for(int i = 0; i < k; i++){
            clusters[i].recenter();
            if(iter<8)clusters[i].clearList();
        }
    }




    // a.insertPoint(p2);
    // a.insertPoint(p3);

    // a.recenter();
    clusters[0].print();
    return 0;
}