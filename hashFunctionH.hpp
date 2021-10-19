
class HashFunctionH{
public:
	explicit HashFunctionH(int w, int dims, int seed);
	~HashFunctionH();
	void printVec();
	int computeH(std::vector<double>& p);
private:
	double getRandom();
	int w;
	std::normal_distribution<double>* distr;
	std::default_random_engine* gen;
	std::vector<double> v;
};