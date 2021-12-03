#pragma once

#include <string>
#include <vector>
#include <map>

class Point;

/* This module handles anything that has to do with reading input from files
as well as the arguements given by the user when executing the program.*/


//Adds every point from file "fileName" to the vector "points". Returns the dimension of points, negative on error
int readDataSet(std::string& fileName, char delimiter, std::vector<Point*>& points);

//Parses the argc/argv results of main, negative on error
int readLshArguments(int argc, char **argv, std::string &inputFile, std::string &queryFile, int &k, int &l,
                    std::string &outputFile, int &numOfNearest, double &radius);

//Parses the argc/argv results of main, negative on error
int readHyperArguments(int argc, char **argv, std::string &inputFile, std::string &queryFile, int &k, int &m, int &probes,
                    std::string &outputFile, int &numOfNearest, double &radius);

//Parses the argc/argv results of main, negative on error
int readClusterArguments(int argc, char **argv, std::string &inputFile, std::string &configFile, bool &complete,
                    std::string &outputFile, std::string &method);

//Reads the config file "fileName", negative values on error
int readClusterConfig(const std::string &fileName, int &clusters, int &L, int &k,
                  int &M, int &d, int &probes);