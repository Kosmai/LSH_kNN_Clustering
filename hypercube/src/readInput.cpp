#include <algorithm>
#include "../inc/point.hpp"
#include "../inc/readInput.hpp"
#include "../inc/LSH.hpp"

void printVec(std::vector<double> v) {
    std::cout << "Vector v" << std::endl << "=================" << std::endl;
    for (auto i: v) {
        std::cout << "|-- > " << i << std::endl;
    }
    std::cout << "=================" << std::endl;
}

int readDataSet(std::string& fileName, char delimiter, LSH& lsh) {
    std::string lineBuffer;
    std::ifstream dataSetFile(fileName);

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
            if(valueBuffer[0] < '0' || valueBuffer[0] > '9'){
                continue;
            }
            vec.push_back(std::stod(valueBuffer));
        }
        Point* p = new Point(vec, item_id);
        lsh.addPoint(p);

    }

    return 0;
}

int readDataSet(std::string& fileName, char delimiter, std::vector<Point>& queries) {
    std::string lineBuffer;
    std::ifstream dataSetFile(fileName);

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
            if(valueBuffer[0] < '0' || valueBuffer[0] > '9'){
                continue;
            }
            vec.push_back(std::stod(valueBuffer));
        }
        Point p = Point(vec, item_id);
        queries.push_back(p);

    }

    return 0;
}

int readQuery(std::string fileName) {
    return 0;
}

int readLshConfig(const std::string &fileName, std::map<std::string, bool> &argumentsRed, std::string &inputFile, std::string &queryFile, int &k, int &l,
                    std::string &outputFile, int &numOfNearest, double &radius){

    std::string lineBuffer;
    std::ifstream dataSetFile(fileName);

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
        if(value[0] == 0){
            continue; //no value assigned in config file (only spaces)
        }

        //read arguments
        if (std::string(param).compare("hash_h_amount") == 0) {
            if((k = atoi(value.c_str())) == 0) return -1;
            argumentsRed["-k"] = true;
        } else if (std::string(param).compare("tables_amount") == 0) {
            if((l = atoi(value.c_str())) == 0) return -1;
            argumentsRed["-L"] = true;
        } else if (std::string(param).compare("nearest_amount") == 0) {
            if((numOfNearest = atoi(value.c_str())) == 0) return -1;
            argumentsRed["-N"] = true;
        } else if (std::string(param).compare("max_radius") == 0) {
            if((radius = atof(value.c_str())) == 0) return -1;
            argumentsRed["-R"] = true;
        } else if (std::string(param).compare("dataset") == 0) {
            inputFile = value;
            argumentsRed["-i"] = true;
        } else if (std::string(param).compare("output") == 0) {
            outputFile = value;
            argumentsRed["-o"] = true;
        } else if (std::string(param).compare("query") == 0) {
            queryFile = value;
            argumentsRed["-q"] = true;
        } else {
            std::cout << "Invalid parameter (" << param << ") in config file. Ignored." << std::endl;
        }
    }
    return 0;
}