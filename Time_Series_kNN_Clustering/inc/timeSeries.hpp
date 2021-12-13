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
    int setVector(std::vector<Observation> observations);

    double discreteFrechetDistance(TimeSeries* otherTs);
    double discreteFrechetDistance(std::vector<Observation>& otherObservations);
    double continuousFrechetDistance(TimeSeries* otherTs);
    double continuousFrechetDistance(std::vector<Observation>& otherObservations);
};

std::vector<Observation> meanCurve(std::vector<Observation> obs1, std::vector<Observation> obs2, double& frechetDistance);
std::vector<Observation> meanCurve(std::vector<std::vector<Observation>>& obs);