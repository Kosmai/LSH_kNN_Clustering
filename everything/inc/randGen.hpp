
/*This module is responsible for all random number generations
that are required for any non-deterministic parts of the
algorithms. setRandomSeed MUST be called to initialize the 
seed before any of the other functions are called. */

//Only call this once
void setRandomSeed(unsigned int seed);

//Returns a normal distribution random double in [0,1]
double getNormalRandom();

//Returns a uniform distribution random float in [0,1]
float getUniformRandomFloat();

//Returns a uniform random int in [-2^31 , 2^31-1]
int getUniformRandom();

//Returns 0/1 uniformly
int tossCoin();