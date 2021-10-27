#include <vector>

class HashFunctionH;

class HashFunctionG{
public:

	HashFunctionG() = default;

	explicit HashFunctionG(int k, int w, int dims);

	HashFunctionG(const HashFunctionG& copy) = delete;

	HashFunctionG& operator=(const HashFunctionG &copy);

	~HashFunctionG();



	//Returns the g(p) = r(i) dot h(p) for all h's
	unsigned int computeID(std::vector<double>& p);

	//Returns g(p) MOD tableSize
	unsigned int computeG(std::vector<double>& p, unsigned int& tableSize);

private:
	int k;
	int w;
	int dims;
	std::vector<int> r;
	HashFunctionH* h;
	
	//mod opperation extended so it works for negatives too
	unsigned int modulo(long a,unsigned int b);
	
};