#pragma once
#include <vector>
#include <list>
#include <cfloat>

class HashTable;
class HashFunctionF;
class Point;
struct Neighbor;
class Cluster;

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

	//
	void bruteForceSearch(Point &queryPoint);

	//Returns N nearest neighbors or within R
    int hyperSearch(Point& queryPoint, int M, int probes);

public:

	Hypercube();

	explicit Hypercube(int dims, int buckets, int L, int k, int w);

	~Hypercube();

	Hypercube(const Hypercube& copy);

	Hypercube& operator=(const Hypercube& copy) = delete;

	//Add a point p to the Hypercube class
	int addPoint(Point* p);
	
	//print specified HashTable
	void printHT(int id);

	//print all HashTables
	void printAllHT();

    //Print the results
	void displayResults(Point &queryPoint, FILE* fp, unsigned int numOfNN, double r);

	//
	int calculateNN(Point &queryPoint, FILE* fp, int M, int probes, unsigned int numOfNN, double r);

    void getNearestByR(double r, int rangeIndex, Cluster* clusters, int currentCluster, int probes, int M);

	static double calculateW(std::vector<Point*> &points);

	double averageRatio = 0;
	double worstDistance = DBL_MIN;
	int distanceOver2 = 0;
};