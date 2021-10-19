#include <random>


static std::default_random_engine generator;
static std::normal_distribution<double> normDistribution(0.0,1.0);
static std::uniform_int_distribution<> uniformDistribution(-2147483648, 2147483647);

void setRandomSeed(unsigned seed){
	generator.seed(seed);
	srand(seed);
}

double getNormalRandom(){
	return normDistribution(generator);
}

int getUniformRandom(){
	return uniformDistribution(generator);
}
