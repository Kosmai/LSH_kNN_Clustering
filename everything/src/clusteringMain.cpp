#include <iostream>
#include <list>
#include <string>
#include <algorithm>
#include <random>
#include <cfloat>
#include <chrono>
#include "../inc/readInput.hpp"
#include "../inc/point.hpp"
#include "../inc/randGen.hpp"
#include "../inc/cluster.hpp"
#include "../inc/kmeans.hpp"
#include "../inc/LSH.hpp"

#define DIMS 128
#define CLUSTERS 3

int main() {
    setRandomSeed(time(NULL));

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    std::string inputFile = "datasets/input_small_id";

    Kmeans kmeans = Kmeans(DIMS, CLUSTERS);

    readDataSet(inputFile, ' ', kmeans);

    auto t1 = high_resolution_clock::now();
    //kmeans.computeLoyd(0.005, 100, PlusPlus);
    auto t2 = high_resolution_clock::now();

    auto t3 = high_resolution_clock::now();
    //kmeans.computeLSH(inputFile, 55000, 50, Random);
    kmeans.computeHypercube(55000, 50, Random);
    auto t4 = high_resolution_clock::now();

    auto loyd = duration_cast<milliseconds>(t2 - t1);
    auto lsh = duration_cast<milliseconds>(t4 - t3);
    std::cout << loyd.count() << std::endl;
    std::cout << lsh.count() << std::endl;

    kmeans.displaySilhouette();
    return 0;
}