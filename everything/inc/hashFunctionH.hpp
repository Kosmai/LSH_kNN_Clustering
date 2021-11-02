#include <vector>
#include <map>

class HashFunctionH{
public:

    HashFunctionH() = default;

    explicit HashFunctionH(int w, int dims);

    HashFunctionH(const HashFunctionH& copy) = default;

    HashFunctionH& operator=(const HashFunctionH &copy) = default;

    ~HashFunctionH() = default;



    void printVec();

    int computeH(std::vector<double>& p);

    int computeHyperH(std::vector<double>& p);

private:
    int w;
    int t;
    std::vector<double> v;
    std::map<int, int> bucketBit;
};