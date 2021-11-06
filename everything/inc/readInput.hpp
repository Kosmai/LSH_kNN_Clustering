#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <map>

class Kmeans;
class LSH;
class Hypercube;
class Point;

int readDataDimensions(std::string& fileName, int& dimension, char delimiter);

int readDataSet(std::string& fileName, char delimiter, Kmeans& kmeans);

int readDataSet(std::string &fileName, char delimiter, LSH &lsh);

int readDataSet(std::string &fileName, char delimiter, Hypercube &hyper);

int readDataSet(std::string& fileName, char delimiter, std::vector<Point*>& points);

int readDataSet(std::string& fileName, char delimiter, std::vector<Point>& queries);

int readQuery(std::string&);

int readLshArguments(int argc, char **argv, std::string &inputFile, std::string &queryFile, int &k, int &l,
                    std::string &outputFile, int &numOfNearest, double &radius);

int readHyperArguments(int argc, char **argv, std::string &inputFile, std::string &queryFile, int &k, int &m, int &probes,
                    std::string &outputFile, int &numOfNearest, double &radius);

int readClusterArguments(int argc, char **argv, std::string &inputFile, std::string &configFile, bool complete,
                    std::string &outputFile, std::string &method);

int readLshConfig(const std::string &fileName, std::map<std::string, bool> &argumentsRed, std::string &inputFile, std::string &queryFile, int &k, int &l,
                    std::string &outputFile, int &numOfNearest, double &radius);

int readHypercubeConfig(const std::string &fileName, std::map<std::string, bool> &argumentsRed, std::string &inputFile,
                  std::string &queryFile, int &k, int &m, int &probes,
                  std::string &outputFile, int &numOfNearest, double &radius);

int readClusterConfig(const std::string &fileName, int &clusters, int &L, int &k,
                  int &M, int &d, int &probes);