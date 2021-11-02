#include <random>

static std::default_random_engine generator;
static std::normal_distribution<double> normDistribution(0.0,1.0);
static std::uniform_real_distribution<> uniformFloatDistribution(0.0, 1.0);
static std::uniform_int_distribution<>  uniformDistribution(-2147483648, 2147483647);

//Needs to be called only once
void setRandomSeed(unsigned int seed){
	generator.seed(seed);
	srand(seed);
}

double getNormalRandom(){
	return normDistribution(generator);
}

int getUniformRandom(){
	return uniformDistribution(generator);
}

float getUniformRandomFloat(){
	return uniformFloatDistribution(generator);
}

int tossCoin() {
    return rand() % 2;
}
