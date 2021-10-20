#include <iostream>
#include <vector>
#include <chrono>
#include <random>
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


int main() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    setRandomSeed(seed);

    std::vector<Point> points;
	std::vector<double> p[MAX_ELEMENTS];
	for(int i = 0; i < MAX_ELEMENTS; i++){
		for(int j = 0; j < DIMS; j++){
			p[i].push_back(getNormalRandom()*10);
		}
        points.push_back(Point(p[i], "temp"));
	}

    LSH lsh = LSH(DIMS, BUCKETS, L, K, W);
    for(int i = 0; i < MAX_ELEMENTS; i++){
        lsh.addPoint(points[i]);
    }
    lsh.printAllHT();

    return 0;
}
