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
#include "kmeans.hpp"

#define DIMS 128

int main() {
    setRandomSeed(time(NULL));

    std::string inputFile = "datasets/input_small_id";
    std::string queryFile = "datasets/query_small_id";

    Kmeans kmeans(128, 3);
    std::vector < Point * > points;
    readDataSet(inputFile, ' ', kmeans);

    kmeans.computeLoyd(0.005, 50, Random);
    kmeans.displaySilhouette();

    return 0;
}