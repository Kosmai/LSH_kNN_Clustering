#include <vector>

class HashFunctionH;

class HashFunctionF{
public:

	HashFunctionF() = default;

	explicit HashFunctionF(int k, int w, int dims);

	HashFunctionF(const HashFunctionF& copy) = delete;

	HashFunctionF& operator=(const HashFunctionF &copy);

	~HashFunctionF();

	unsigned int computeID(std::vector<double>& p);

	unsigned int computeF(std::vector<double>& p, unsigned int& tableSize);

private:
	int k;
	int w;
	int dims;
	std::vector<int> r;
	HashFunctionH* h;
};