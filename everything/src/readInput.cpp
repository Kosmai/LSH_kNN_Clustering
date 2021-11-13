#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../inc/point.hpp"
#include "../inc/readInput.hpp"


int readDataSet(std::string &fileName, char delimiter, std::vector<Point *> &points) {

    unsigned int dimension = 0;

    std::string lineBuffer;
    std::ifstream dataSetFile(fileName);

    //check if stream was successfully created
    if(dataSetFile.fail()){
        return -1;
    }
    
    //read each line
    while (std::getline(dataSetFile, lineBuffer)) {
        std::string item_id;
        std::vector<double> vec;

        std::istringstream lineStream(lineBuffer);
        std::getline(lineStream, item_id, delimiter);
        std::string valueBuffer;

        //read every value of the vector
        while (std::getline(lineStream, valueBuffer, delimiter)) {
            //make sure there is a number in valueBuffer
            if ((valueBuffer[0] < '0' || valueBuffer[0] > '9') && valueBuffer[0] != '-') {
                continue;
            }
            vec.push_back(std::stod(valueBuffer));
        }
        if(dimension == 0){
            //initialize dimension the first time
            dimension = vec.size();
        }
        else if(dimension != vec.size()){
            //inaccurate point dimension read
            return -1;
        }
        //add point in vector
        Point *p = new Point(vec, item_id);
        points.push_back(p);

    }

    return (int)dimension;
}

int readLshArguments(int argc, char **argv, std::string &inputFile, std::string &queryFile, int &k, int &l,
                     std::string &outputFile, int &numOfNearest, double &radius) {


    //read arguments
    for (int i = 1; i < argc; i += 2) {
        if (std::string(argv[i]).compare("-i") == 0 && i + 1 < argc) {
            inputFile = argv[i + 1];
        } else if (std::string(argv[i]).compare("-q") == 0 && i + 1 < argc) {
            queryFile = argv[i + 1];
        } else if (std::string(argv[i]).compare("-k") == 0 && i + 1 < argc) {
            k = atoi(argv[i + 1]);
        } else if (std::string(argv[i]).compare("-L") == 0 && i + 1 < argc) {
            l = atoi(argv[i + 1]);
        } else if (std::string(argv[i]).compare("-o") == 0 && i + 1 < argc) {
            outputFile = argv[i + 1];
        } else if (std::string(argv[i]).compare("-N") == 0 && i + 1 < argc) {
            numOfNearest = atoi(argv[i + 1]);
        } else if (std::string(argv[i]).compare("-R") == 0 && i + 1 < argc) {
            radius = atof(argv[i + 1]);
        } else {
            //unknown argument
            return -1;
        }
    }

    return 0;
}

int readHyperArguments(int argc, char **argv, std::string &inputFile, std::string &queryFile, int &k, int &m, int &probes,
                   std::string &outputFile, int &numOfNearest, double &radius) {

    //read arguments
    for (int i = 1; i < argc; i += 2) {
        if (std::string(argv[i]).compare("-i") == 0 && i + 1 < argc) {
            inputFile = argv[i + 1];
        } else if (std::string(argv[i]).compare("-q") == 0 && i + 1 < argc) {
            queryFile = argv[i + 1];
        } else if (std::string(argv[i]).compare("-k") == 0 && i + 1 < argc) {
            k = atoi(argv[i + 1]);
        } else if (std::string(argv[i]).compare("-M") == 0 && i + 1 < argc) {
            m = atoi(argv[i + 1]);
        } else if (std::string(argv[i]).compare("-probes") == 0 && i + 1 < argc) {
            probes = atoi(argv[i + 1]);
        } else if (std::string(argv[i]).compare("-o") == 0 && i + 1 < argc) {
            outputFile = argv[i + 1];
        } else if (std::string(argv[i]).compare("-N") == 0 && i + 1 < argc) {
            numOfNearest = atoi(argv[i + 1]);
        } else if (std::string(argv[i]).compare("-R") == 0 && i + 1 < argc) {
            radius = atof(argv[i + 1]);
        } else {
            //unknown argument
            return -1;
        }
    }

    return 0;
}


int readClusterArguments(int argc, char **argv, std::string &inputFile, std::string &configFile, bool &complete,
                         std::string &outputFile, std::string &method) {

    //keep track of what arguments have been read
    std::map<std::string, bool> argumentsRed;

    argumentsRed["-i"] = false;
    argumentsRed["-c"] = false;
    argumentsRed["-o"] = false;
    argumentsRed["-m"] = false;

    //read arguments
    for (int i = 1; i < argc; i += 2) {
        if (std::string(argv[i]).compare("-i") == 0 && i + 1 < argc) {
            inputFile = argv[i + 1];
            argumentsRed["-i"] = true;
        } else if (std::string(argv[i]).compare("-c") == 0 && i + 1 < argc) {
            configFile = argv[i + 1];
            argumentsRed["-c"] = true;
        } else if (std::string(argv[i]).compare("-complete") == 0) {
            complete = true;
            i--;
        } else if (std::string(argv[i]).compare("-o") == 0 && i + 1 < argc) {
            outputFile = argv[i + 1];
            argumentsRed["-o"] = true;
        } else if (std::string(argv[i]).compare("-m") == 0 && i + 1 < argc) {
            method = argv[i + 1];
            argumentsRed["-m"] = true;
        } else {
            //unknown argument
            return -1;
        }
    }

    //check for missing required arguments
    for (std::pair<std::string, bool> arg: argumentsRed) {
        if (!arg.second) {
            return -2;
        }
    }

    return 0;
}

int readClusterConfig(const std::string &fileName, int &clusters, int &L, int &k,
                      int &M, int &d, int &probes) {


    //keep track of what arguments have been read
    std::map<std::string, bool> argumentsRed;

    argumentsRed["number_of_clusters"] = false;
    argumentsRed["number_of_vector_hash_tables"] = false;
    argumentsRed["number_of_vector_hash_functions"] = false;
    argumentsRed["max_number_M_hypercube"] = false;
    argumentsRed["number_of_hypercube_dimensions"] = false;
    argumentsRed["number_of_probes"] = false;

    std::string lineBuffer;
    std::ifstream dataSetFile(fileName);

    //check if stream was successfully created
    if(dataSetFile.fail()){
        return -1;
    }

    //read each line
    while (std::getline(dataSetFile, lineBuffer)) {
        std::string param;
        std::string value;

        std::istringstream lineStream(lineBuffer);
        std::getline(lineStream, param, ':');
        std::getline(lineStream, value);
        std::string::iterator end_pos = std::remove(value.begin(), value.end(), ' ');
        value.erase(end_pos, value.end());
        end_pos = std::remove(value.begin(), value.end(), '\n');
        value.erase(end_pos, value.end());
        if (value[0] == 0) {
            //no value assigned in config file, skip this
            continue;
        }

        //read arguments
        if (std::string(param).compare("number_of_clusters") == 0) {
            if ((clusters = atoi(value.c_str())) == 0) return -1;
            argumentsRed["number_of_clusters"] = true;
        } else if (std::string(param).compare("number_of_vector_hash_tables") == 0) {
            if ((L = atoi(value.c_str())) == 0) return -1;
            argumentsRed["number_of_vector_hash_tables"] = true;
        } else if (std::string(param).compare("number_of_vector_hash_functions") == 0) {
            if ((k = atoi(value.c_str())) == 0) return -1;
            argumentsRed["number_of_vector_hash_functions"] = true;
        } else if (std::string(param).compare("max_number_M_hypercube") == 0) {
            if ((M = atoi(value.c_str())) == 0) return -1;
            argumentsRed["max_number_M_hypercube"] = true;
        } else if (std::string(param).compare("number_of_hypercube_dimensions") == 0) {
            if ((d = atoi(value.c_str())) == 0) return -1;
            argumentsRed["number_of_hypercube_dimensions"] = true;
        } else if (std::string(param).compare("number_of_probes") == 0) {
            if ((probes = atoi(value.c_str())) == 0) return -1;
            argumentsRed["number_of_probes"] = true;
        } else {
            std::cout << "Invalid parameter (" << param << ") in config file. Ignored." << std::endl;
        }
    }

    //check for missing required arguments
    for (std::pair<std::string, bool> arg: argumentsRed) {
        if (!arg.second) {
            return -2;
        }
    }

    return 0;
}