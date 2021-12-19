#include <vector>
#include <string>

class Point;

/* This module implements the Time Series class that represents a
time series (polygonal curve) */

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
    void print();   // used for debugging purposes

    // snapping and filtering
    Point* snapToGrid(double dx, double dy, double tx = 0, double ty = 0);
    Point* filter(double e, bool consecutiveErases = false, double delta = 1, double t = 0.05);


    //frechet distance
    double discreteFrechetDistance(TimeSeries* otherTs);
    double discreteFrechetDistance(std::vector<Observation>& otherObservations);
    double continuousFrechetDistance(TimeSeries* otherTs, bool filterQueries = false);
    double continuousFrechetDistance(std::vector<Observation>& otherObservations, bool filterQueries = false, double e = 1);

    std::vector<Observation>& getVector();
    int setVector(std::vector<Observation> observations);
};

// calculation of mean curves (used for clustering while updating centroids)
std::vector<Observation> meanCurve(std::vector<Observation> obs1, std::vector<Observation> obs2, double& frechetDistance);
std::vector<Observation> meanCurve(std::vector<std::vector<Observation>>& obs);

// used to filter curves for continuous frechet distance (optional)
std::vector<Observation> filterCurve(std::vector<Observation>& obs, double e, bool consecutiveErases = true);