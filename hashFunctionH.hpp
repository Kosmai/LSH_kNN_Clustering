#include <vector>

class HashFunctionH{
public:
	HashFunctionH();
	explicit HashFunctionH(int w, int dims);
	~HashFunctionH();
	void printVec();
	int computeH(std::vector<double>& p);
private:
	int w;
	int t;
	std::vector<double> v;
};