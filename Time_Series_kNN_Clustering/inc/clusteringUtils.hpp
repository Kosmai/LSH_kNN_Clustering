#include <iostream>
#include <string>

int getMetric(std::string update);
void displayClusterParameters(std::string &inputFile, std::string &configFile, std::string &outputFile, 
                         std::string &update, std::string &assignment, bool &complete, bool &silhouette,
                         int k, int L, int M, int d, int w, int probes, int clusters);