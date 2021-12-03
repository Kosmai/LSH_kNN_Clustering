#include <vector>

class HashFunctionH;

class HashFunctionF{
private:
	int k;
	int w;
	int dims;
	std::vector<int> r;
	HashFunctionH* h;

public:
	//constructors-destructors
	HashFunctionF() = default;
	explicit HashFunctionF(int k, int w, int dims);
	HashFunctionF(const HashFunctionF& copy) = delete;
	HashFunctionF& operator=(const HashFunctionF &copy);
	~HashFunctionF();

	//Returns the f(p) using multiple h(p) results
	unsigned int computeF(std::vector<double>& p);

	//Returns f(p) MOD tableSize
	unsigned int computeID(std::vector<double>& p, unsigned int& tableSize);
};