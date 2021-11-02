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

    Kmeans kmeans = Kmeans(DIMS, CLUSTERS);

    //readDataSet(inputFile, ' ', kmeans);

    //kmeans.computeLoyd(0.005, 100, PlusPlus);
    //kmeans.displaySilhouette();

    //lsh.calculateNN(queries[0], 10, 350);
    kmeans.computeLSH(inputFile, 55000, 50, Random);
    return 0;
}