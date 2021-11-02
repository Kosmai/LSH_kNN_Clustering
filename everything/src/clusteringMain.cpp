#include <iostream>
#include <list>
#include <string>
#include <algorithm>
#include <random>
#include <cfloat>
#include "../inc/readInput.hpp"
#include "../inc/point.hpp"
#include "../inc/randGen.hpp"
#include "../inc/cluster.hpp"
#include "../inc/kmeans.hpp"
#include "../inc/LSH.hpp"

#define DIMS 128
#define CLUSTERS 5

int main() {
    setRandomSeed(time(NULL));

    std::string inputFile = "datasets/input_small_id";
    std::string queryFile = "datasets/query_small_id";

    Kmeans kmeans = Kmeans(DIMS, CLUSTERS);
    std::vector<Point> queries;
    //readDataSet(inputFile, ' ', kmeans);
    readDataSet(queryFile, ' ', queries);

    //kmeans.computeLoyd(0.005, 100, PlusPlus);
    //kmeans.displaySilhouette();

    LSH lsh = LSH(DIMS, 1000, 20, 4, 200);
    readDataSet(inputFile, ' ', lsh);
    std::list<Point*> rad;
    //lsh.calculateNN(queries[0], 10, 350);
    lsh.getNearestByR(queries[0], 400, rad);
    for(auto item : rad){
        std::cout << item->getId() << std::endl;
    }

    return 0;
}