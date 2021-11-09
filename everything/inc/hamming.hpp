#include <list>

/* This module includes the Hamming class which is responsible for
generating all possible ints with given hamming distance and returning
them one by one. */

class Hamming{
private:
	unsigned int base;         //the starting int we want hamming permutes on
	int k;                     //max amount of bits
	int currentDist;
	std::list<int> masks;      //list of integers serving as masks

    //fills masks with all ints with given hamming distance
	void fillHammingMasks(int hammingDistance);

    //calls fill hamming masks with increased by one distance
	void nextDistance();
public:

	//make sure k is the number of bits "base" needs to be represented
	Hamming(unsigned int base, int k);

    //prints the list of masks currently stored
    void printMaskList();

    //returns the next closest hamming permutation
    unsigned int getNext();
};