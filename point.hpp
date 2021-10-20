#pragma once

#include <vector>
#include <iostream>

class Point {
private:
    std::string id;
    std::vector<double> vec;
public:
    Point();

    Point(std::vector<double>, std::string);

    ~Point();

    Point& operator=(const Point &copy);

    int setId(std::string);

    int setVector(std::vector<double>);

    std::string getId();

    std::vector<double> getVector();

    int getDimension();

    double l2Distance(Point*);

    double l2Distance(std::vector<double>);

    void print();

};