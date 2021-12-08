#pragma once

#include <vector>
#include <list>
#include <cfloat>

class HashTable;
class HashFunctionG;
class Point;
class Cluster;
class TimeSeries;

/* This module implements the LSH class, which holds all the 
necessary information and methods used in the LSH algorithm */

//used to store distance information about neighbors found
struct Neighbor {
    Point *point;
    double distance;
};

class LSH{
private:
	int dims;    //amount of dimensions used
	int buckets; //amount of buckets per hashtable
	int L;       //amount of diffrent hashtables
	int k;       //amount of diffrent H functions in each G function
	int w;       //determines the size of the gap used in H funtions

	HashTable*     hashTables;
	HashFunctionG* gFunctions;
	std::list<Point*> points;
	std::list<Neighbor*> LSHNeighbors;
	std::list<Neighbor*> realNeighbors;

	//uses brute force to store nearest neighbors in realNeighbors
	void bruteForceSearch(Point &queryPoint, int metric);

	//finds nearest neighbors and stores them in LSHNeighbors
    int LSHSearch(Point& queryPoint, int metric);

    //print the results in output file fp
	void displayResults(Point &queryPoint, FILE* fp, unsigned int numOfNN);

public:
	//constructors-destructors
	LSH();
	explicit LSH(int dims, int buckets, int L, int k, int w);
	~LSH();
	LSH(const LSH& copy);
	LSH& operator=(const LSH& copy) = delete;

	//add a point p to the LSH class
	int addPoint(Point* p);

	//add a time series to the LSH class (as grid-mapped point)
	int addTimeSeries(TimeSeries* t, double dx, double dy, double filter_e = -1.0);

	//returns the list of points stored in the class
	std::list<Point*>& getPoints();

	//uses private search methods and filters their results using r (radius) and numOfNN
	//outputs the results in the output file fp
	int calculateNN(Point &queryPoint, FILE* fp, unsigned int numOfNN, double r, int metric);

	//used for clustering
	void getNearestByR(double r , Cluster* clusters, int currentCluster);

	//dynamic calculation of the w parameter
	static double calculateW(std::vector<Point*> &points);


	//used for testing purposes

	//print specified HashTable
	void printHT(int id);

	//print all HashTables
	void printAllHT();

	//resets the statistics below
	void resetStatistics();

	double averageRatio   = 0;
	double worstDistance  = DBL_MIN;
	int successfulQueries = 0;
};