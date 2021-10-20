#include <vector>

class HashFunctionH;

class HashFunctionG{
public:
	HashFunctionG();
	explicit HashFunctionG(int k, int w, int dims);
	~HashFunctionG();

	HashFunctionG& operator=(const HashFunctionG &copy);
	unsigned int computeID(std::vector<double>& p);
	unsigned int computeG(std::vector<double>& p, unsigned int tableSize);
private:
	unsigned int modulo(long a,unsigned int b);
	int k;
	int w;
	int dims;
	std::vector<int> r;
	HashFunctionH* h;
};