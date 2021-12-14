#pragma once

#include <vector>
#include <list>
#include <cfloat>

class HashTable;
class HashFunctionF;
class Point;
struct Neighbor;
class Cluster;

/* This module implements the Hypercube class, which holds all the 
information and methods necessary for the Hypercube algorithm */

class Hypercube{
private:
	int dims;    //amount of dimensions used
	int buckets; //amount of buckets per hashtable
	int L;       //amount of diffrent hashtables
	int k;       //amount of diffrent H functions in each G function
	int w;       //determines the size of the gap used in H funtions
	HashTable*     hashTables;
	HashFunctionF* fFunctions;
	std::list<Point*> points;
	std::list<Neighbor*> HyperNeighbors;
	std::list<Neighbor*> realNeighbors;

	//uses brute force to store nearest neighbors in realNeighbors
	void bruteForceSearch(Point &queryPoint);

	//finds nearest neighbors and stores them in LSHNeighbors
    int hyperSearch(Point& queryPoint, int M, int probes);

    //print the results in output file fp
	void displayResults(Point &queryPoint, FILE* fp, unsigned int numOfNN, double r);

public:

	//constructors-destructors
	Hypercube();
	explicit Hypercube(int dims, int buckets, int L, int k, int w);
	~Hypercube();
	Hypercube(const Hypercube& copy);
	Hypercube& operator=(const Hypercube& copy) = delete;

	//Add a point p to the Hypercube class
	int addPoint(Point* p);

	//returns the list of points stored in the class
	std::list<Point*>& getPoints();

	//uses private search methods and filters their results using r (radius) and numOfNN
	//outputs the results in the output file fp
	int calculateNN(Point &queryPoint, FILE* fp, int M, int probes, unsigned int numOfNN, double r);

	//used for clustering
    void getNearestByR(double r, Cluster* clusters, int currentCluster, int probes, int M);

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
	double totalTimeApproximate; //used to compute average time per query
	double totalTimeTrue;        //same for brute force
};