#pragma once

#include <vector>

/* This module is responsible for the representation of points using
the Point class and provides basic functionality, s.a. calculating the
distance between two points, assigning cluster index to point etc */

class Point {
private:
    std::string id;
    std::vector<double> vec;

    //used in clustering
    int clusterIndex;

public:
    //Constructors-Destructors
    Point();
    Point(std::vector<double> &vec, const std::string &id);
    ~Point() = default;
    Point(const Point &copy) = default;
    Point &operator=(const Point &copy);

    //setters
    int setId(std::string &id);
    int setVector(std::vector<double> &);
    int setClusterIndex(int clusterIndex);

    //getters
    std::string getId();
    std::vector<double> &getVector();
    int getClusterIndex();
    int getDimension();

    //checks if two points have the same id
    bool operator==(Point &p);
    
    //calculates the distance between two points
    double l2Distance(Point *);
    double l2Distance(std::vector<double> &);

    //for testing purposes
    void print();
    void printInfo();
};