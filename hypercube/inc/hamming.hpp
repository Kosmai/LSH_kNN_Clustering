#include <list>

class Hamming{
private:
	unsigned int base; //the starting int we want hamming permutes on
	int k;   //max amount of bits
	int currentDist;
	std::list<int> masks;
	void fillHammingMasks(int hammingDistance);
	void nextDistance();
public:

	//make sure k is the number of bits "base" needs to be represented
	Hamming(unsigned int base, int k); //base number will be permutated using hamming
	void printMaskList();  //prints the list of masks currently stored
	unsigned int getNext(); //returns the next closest hamming permutation
};