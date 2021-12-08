#include <vector>
#include <string>

class Point;

struct Observation{
    double x;
    double y;
};

struct Leash{
    int i;
    int j;
};

class TimeSeries{
private:
    std::string id;
    std::vector<Observation> observations;
public:
    TimeSeries(Point* point);
    ~TimeSeries() = default;
    void print();
    Point* snapToGrid(double dx, double dy);
    Point* filter(double e, bool consecutiveErases = false);

    std::vector<Observation>& getVector();

    double discreteFrechetDistance(TimeSeries*);
    double discreteFrechetDistance(std::vector<Observation>&);
};