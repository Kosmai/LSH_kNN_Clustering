
//Only call this once with any seed
void setRandomSeed(unsigned int seed);



//Returns a normal distribution random double in [0,1]
double getNormalRandom();

//Returns a uniform distribution random float in [0,1]
float getUniformRandomFloat();

//Returns a uniform random int in [-2^31 , 2^31-1]
int getUniformRandom();

//Returns 0 or 1 randomly
int tossCoin();