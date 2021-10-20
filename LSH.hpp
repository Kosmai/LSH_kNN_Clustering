#include <vector>
class HashTable;
class HashFunctionG;
class Point;

class LSH{
private:
	int k;       //amount of diffrent H functions in each G function
	int w;       //determines the size of the gap used in H funtions
	int L;       //amount of diffrent hashtables
	int dims;    //amount of dimensions used
	int buckets; //amount of buckets per hashtable
	HashTable*     hashTables;
	HashFunctionG* gFunctions;
	std::list<Point> points;

public:
	LSH(int dims, int buckets, int L, int k, int w);
	~LSH();
	void printHT(int id);
	void printAllHT();
	int addPoint(Point& p);


};