#pragma once

#include <vector>
#include <iostream>

class Point {
private:
    std::string id;
    std::vector<double> vec;
    int clusterIndex;
public:
    Point();

    Point(std::vector<double> &vec, const std::string &id);

    ~Point() = default;

    Point(const Point &copy) = default;

    Point &operator=(const Point &copy);


    int setId(std::string &id);

    int setVector(std::vector<double> &);

    int setClusterIndex(int clusterIndex);

    std::string getId();

    std::vector<double> &getVector();

    int getClusterIndex();

    int getDimension();

    bool operator==(Point &p);

    double l2Distance(Point *);

    double l2Distance(std::vector<double> &);

    void print();

};