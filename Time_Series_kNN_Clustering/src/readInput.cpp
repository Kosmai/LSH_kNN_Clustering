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

int readSearchArguments(int argc, char** argv, std::string &inputFile, std::string &queryFile, int &k, int &l, int &m, int &probes,
    std::string &outputFile, std::string &algorithm, std::string &metric, double &delta, bool &disableBruteForce) {

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
        } else if (std::string(argv[i]).compare("-M") == 0 && i + 1 < argc) {
            m = atoi(argv[i + 1]);
        } else if (std::string(argv[i]).compare("-probes") == 0 && i + 1 < argc) {
            probes = atoi(argv[i + 1]);
        } else if (std::string(argv[i]).compare("-o") == 0 && i + 1 < argc) {
            outputFile = argv[i + 1];
        } else if (std::string(argv[i]).compare("-algorithm") == 0 && i + 1 < argc) {
            algorithm = argv[i + 1];
        } else if (std::string(argv[i]).compare("-metric") == 0 && i + 1 < argc) {
            metric = argv[i + 1];
        } else if (std::string(argv[i]).compare("-delta") == 0 && i + 1 < argc) {
            delta = atof(argv[i + 1]);
        } else if (std::string(argv[i]).compare("-disableBF") == 0) {
            disableBruteForce = true;
            i--;
        } else {
            //unknown argument
            return -1;
        }
    }

    if(inputFile == ""){
        std::cout << "Give path to input file: ";
        std::cin >> inputFile;
    }

    if(algorithm == ""){
        std::cout << "Give algorithm (LSH/Hypercube/Frechet): ";
        std::cin >> algorithm;
    }

    if(algorithm == "Frechet" && metric == ""){
        std::cout << "Give metric (discrete/continuous): ";
        std::cin >> metric;
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


int readClusterArguments(int argc, char **argv, std::string &inputFile, std::string &configFile, std::string &outputFile, 
                         std::string &update, std::string &assignment, bool &complete, bool &silhouette) {

    //keep track of what arguments have been read
    std::map<std::string, bool> argumentsRed;

    //argumentsRed["-i"] = false;
    //argumentsRed["-c"] = false;
    //argumentsRed["-o"] = false;
    argumentsRed["-assignment"] = false;
    argumentsRed["-update"] = false;

    //read arguments
    for (int i = 1; i < argc; i += 2) {
        if (std::string(argv[i]).compare("-i") == 0 && i + 1 < argc) {
            inputFile = argv[i + 1];
            argumentsRed["-i"] = true;
        } else if (std::string(argv[i]).compare("-c") == 0 && i + 1 < argc) {
            configFile = argv[i + 1];
            argumentsRed["-c"] = true;
        } else if (std::string(argv[i]).compare("-o") == 0 && i + 1 < argc) {
            outputFile = argv[i + 1];
            argumentsRed["-o"] = true;
        } else if (std::string(argv[i]).compare("-update") == 0 && i + 2 < argc) {
            update = argv[i + 1];
            update += argv[i + 2];
            i++;
            argumentsRed["-update"] = true;
        } else if (std::string(argv[i]).compare("-assignment") == 0 && i + 1 < argc) {
            assignment = argv[i + 1];
            argumentsRed["-assignment"] = true;
        } else if (std::string(argv[i]).compare("-complete") == 0) {
            complete = true;
            i--;
        } else if (std::string(argv[i]).compare("-silhouette") == 0) {
            silhouette = true;
            i--;
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
        } else if (std::string(param).compare("number_of_vector_hash_tables") == 0) {
            if ((L = atoi(value.c_str())) == 0) return -1;
        } else if (std::string(param).compare("number_of_vector_hash_functions") == 0) {
            if ((k = atoi(value.c_str())) == 0) return -1;
        } else if (std::string(param).compare("max_number_M_hypercube") == 0) {
            if ((M = atoi(value.c_str())) == 0) return -1;
        } else if (std::string(param).compare("number_of_hypercube_dimensions") == 0) {
            if ((d = atoi(value.c_str())) == 0) return -1;
        } else if (std::string(param).compare("number_of_probes") == 0) {
            if ((probes = atoi(value.c_str())) == 0) return -1;
        } else {
            std::cout << "Invalid parameter (" << param << ") in config file. Ignored." << std::endl;
        }
    }

    return 0;
}