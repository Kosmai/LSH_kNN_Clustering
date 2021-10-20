#include "readInput.hpp"

void printVec(std::vector<double> v) {
    std::cout << "Vector v" << std::endl << "=================" << std::endl;
    for (auto i: v) {
        std::cout << "|-- > " << i << std::endl;
    }
    std::cout << "=================" << std::endl;
}

int readDataSet(std::string fileName, char delimiter) {
    std::string lineBuffer;
    std::ifstream dataSetFile(fileName);

    //read each line
    while (std::getline(dataSetFile, lineBuffer)) {
        std::string item_id;
        std::vector<double> vec;

        std::istringstream lineStream(lineBuffer);
        getline(lineStream, item_id, delimiter);

        std::string valueBuffer;

        //read every value of the vector
        while (std::getline(lineStream, valueBuffer, delimiter)) {
            vec.push_back(std::stod(valueBuffer));
        }
        printVec(vec);

    }

    return 0;
}

int readQuery(std::string fileName) {
    return 0;
}
