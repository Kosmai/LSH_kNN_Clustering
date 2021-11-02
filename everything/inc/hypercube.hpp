#pragma once
#include <vector>
#include <list>

class HashTable;
class HashFunctionF;
class Point;
struct Neighbor;

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
	std::list<Neighbor*> radiusNeighbors;

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
	void displayResults(Point &queryPoint, unsigned int numOfNN, double r);

	//
	int calculateNN(Point &queryPoint, int M, int probes, unsigned int numOfNN, double r);

};