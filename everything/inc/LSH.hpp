#pragma once

#include <vector>
#include <list>
#include <cfloat>

class HashTable;
class HashFunctionG;
class Point;
class Cluster;

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
	void bruteForceSearch(Point &queryPoint);

	//finds nearest neighbors and stores them in LSHNeighbors
    int LSHSearch(Point& queryPoint);

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

	//returns the list of points stored in the class
	std::list<Point*>& getPoints();

	//uses private search methods and filters their results using r (radius) and numOfNN
	//outputs the results in the output file fp
	int calculateNN(Point &queryPoint, FILE* fp, unsigned int numOfNN, double r);

	//used for clustering
	void getNearestByR(double r, int rangeIndex, Cluster* clusters, int currentCluster);

	//dynamic calculation of the w parameter
	static double calculateW(std::vector<Point*> &points);


	//used for testing purposes

	//print specified HashTable
	void printHT(int id);

	//print all HashTables
	void printAllHT();

	double averageRatio = 0;
	double worstDistance = DBL_MIN;
};