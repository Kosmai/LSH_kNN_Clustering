#include <vector>
#include <list>
class HashTable;
class HashFunctionG;
class Point;
class Cluster;

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
	std::list<Point*> radiusNeighbors;

	//
	void bruteForceSearch(Point &queryPoint);

	//Returns N nearest neighbors or within R
    int LSHSearch(Point& queryPoint);

public:

	LSH();

	explicit LSH(int dims, int buckets, int L, int k, int w);

	~LSH();

	LSH(const LSH& copy);

	LSH& operator=(const LSH& copy) = delete;

	//Add a point p to the LSH class
	int addPoint(Point* p);

	std::list<Point*>& getPoints();

	//print specified HashTable
	void printHT(int id);

	//print all HashTables
	void printAllHT();

    //Print the results
	void displayResults(Point &queryPoint, unsigned int numOfNN, double r);

	//
	int calculateNN(Point &queryPoint, unsigned int numOfNN, double r);

	void getNearestByR(double r, int rangeIndex, Cluster* clusters, int currentCluster);

};