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

#define DIMS 128
#define CLUSTERS 5

int main() {
    setRandomSeed(time(NULL));

    std::string inputFile = "datasets/input_small_id";
    std::string queryFile = "datasets/query_small_id";

    Kmeans kmeans(DIMS, CLUSTERS);
    std::vector < Point * > points;
    readDataSet(inputFile, ' ', kmeans);

    kmeans.computeLoyd(0.005, 100, PlusPlus);
    kmeans.displaySilhouette();

    return 0;
}