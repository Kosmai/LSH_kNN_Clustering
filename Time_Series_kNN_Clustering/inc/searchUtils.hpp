#include <vector>
#include <string>

class Hypercube;
class LSH;
class Point;

/* This module includes some useful functions to assist the search main function */

Hypercube* initializeHypercube(int dims, int k, double &w, std::vector<Point*> &points);
LSH* initializeLSH(int dims, int l, int k, double &w, std::vector<Point*> &points);
LSH* initializeLSH(int dims, int l, int k, double &w, std::vector<TimeSeries*> &ts, double dx, double dy, double filter_e = -1.0);
int initializeInput(std::string &inputFile, int &dims, std::vector<Point*> &points);
void printParameters(int k, int l, int numOfNearest, double radius, int w);
int searchLoop(LSH& lsh, std::string queryFile, std::string outputFile, int numOfNearest, double radius, int metric, double dx, double dy, double filter_e = -1.0, bool disableBruteForce = false, bool filterQueries = false);
int searchLoop(Hypercube& cube, std::string queryFile, std::string outputFile, int probes, int m, int numOfNearest, double radius);
