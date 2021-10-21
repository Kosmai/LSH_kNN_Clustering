#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <map>

int readDataSet(std::string, char);

int readQuery(std::string);

int readLshConfig(const std::string &fileName, std::map<std::string, bool> &argumentsRed, std::string &inputFile, std::string &queryFile, int &k, int &l,
                    std::string &outputFile, int &numOfNearest, double &radius);