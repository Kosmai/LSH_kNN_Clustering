#include <vector>

class HashFunctionH;

class HashFunctionG{
public:
	explicit HashFunctionG(int k, int w, int dims);
	~HashFunctionG();
	int computeG(std::vector<double>& p);
private:
	int modulo(long a,unsigned int b);
	int k;
	int w;
	int dims;
	std::vector<int> r;
	HashFunctionH* h;
};