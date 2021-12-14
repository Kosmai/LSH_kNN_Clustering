#include <iostream>
#include <vector>

#include <map>
#include "../inc/randGen.hpp"
#include "../inc/point.hpp"
#include "../inc/LSH.hpp"
#include "../inc/searchUtils.hpp"
#include "../inc/timeSeries.hpp"
#include "../inc/hashTable.hpp"
#include "../inc/cluster.hpp"
#include <gtest/gtest.h>

/*----------------------*/
/* Point Module Testing */
/*----------------------*/

TEST(PointTest, Construction){
    std::vector<double> vec{1, 2, 3};
    Point a(vec, "point a");

    ASSERT_EQ(a.getVector(), vec);
}

TEST(PointTest, L2Distance){
    std::vector<double> vec1{1, 2, 3};
    Point a(vec1, "point a");

    std::vector<double> vec2{1, 2, 4};
    Point b(vec2, "point b");

    ASSERT_EQ(a.l2Distance(&b), 1);
}

TEST(PointTest, OperatorEQ){
    std::vector<double> vec1{1, 2, 3};
    Point a(vec1, "point a");

    Point b = a;

    ASSERT_EQ(b.getVector(), vec1);
}

/*---------------------------*/
/* TimeSeries Module Testing */
/*---------------------------*/

TEST(TimeSeriesTest, Construction){
    std::vector<double> vec{100, 50};
    Point a(vec, "point a");

    TimeSeries ts(&a);

    ASSERT_EQ(ts.getVector()[0].x, 1);
    ASSERT_EQ(ts.getVector()[1].x, 2);

    ASSERT_EQ(ts.getVector()[0].y, 100);
    ASSERT_EQ(ts.getVector()[1].y, 50);
}

TEST(TimeSeriesTest, DiscreteFrechet){
    std::vector<double> vec1{10, 20, 30};
    Point a(vec1, "point a");

    std::vector<double> vec2{10, 20, 40};
    Point b(vec2, "point a");

    TimeSeries ts1(&a);
    TimeSeries ts2(&b);

    ASSERT_EQ(ts1.discreteFrechetDistance(&ts2), 10);
}

TEST(TimeSeriesTest, ContinuousFrechet){
    std::vector<double> vec1{10, 20, 30};
    Point a(vec1, "point a");

    std::vector<double> vec2{10, 20, 40};
    Point b(vec2, "point a");

    TimeSeries ts1(&a);
    TimeSeries ts2(&b);

    ASSERT_EQ(ts1.continuousFrechetDistance(&ts2), 10);
}

TEST(TimeSeriesTest, SnapToGrid){
    std::vector<double> vec1{10, 10.5, 11};
    Point a(vec1, "point a");

    TimeSeries ts1(&a);

    ASSERT_EQ(ts1.snapToGrid(2, 2)->getVector().size(), 2*vec1.size());
}

TEST(TimeSeriesTest, Filter){
    std::vector<double> vec1{10, 10.5, 11};
    Point a(vec1, "point a");

    TimeSeries ts1(&a);

    ASSERT_EQ(ts1.filter(1)->getVector().size(), vec1.size());
}

/*------------------------*/
/* Cluster Module Testing */
/*------------------------*/

TEST(ClusterTest, Insertion){
    std::vector<double> vec1{10, 10.5, 11};
    Point a(vec1, "point a");

    Cluster cluster(3);

    cluster.insertPoint(&a);

    ASSERT_EQ(cluster.count(), 1);
    ASSERT_EQ(cluster.getClusteredPoints().front()->getId(), "point a");
}

TEST(ClusterTest, Clear){
    std::vector<double> vec{10, 10.5, 11};
    Point a(vec, "point a");
    Point b(vec, "point b");

    Cluster cluster(3);

    cluster.insertPoint(&a);
    cluster.insertPoint(&b);

    ASSERT_EQ(cluster.count(), 2);

    cluster.clearList();

    ASSERT_EQ(cluster.count(), 0);
}

TEST(ClusterTest, Recenter){
    std::vector<double> vec1{1, 1, 1, 1};
    std::vector<double> vec2{2, 2, 2, 2};
    std::vector<double> centroid{100, 200, 300, 400};

    Point a(vec1, "point a");
    Point b(vec2, "point b");
    Point centroidPoint(centroid, "centroid");

    Cluster cluster(4);
    cluster.setCentroid(&centroidPoint);

    cluster.insertPoint(&a);
    cluster.insertPoint(&b);

    cluster.recenter();

    std::vector<double> correctCentroid{1.5, 1.5, 1.5, 1.5};
    ASSERT_EQ(cluster.getCentroid().getVector(), correctCentroid);
}

/*--------------------------*/
/* HashTable Module Testing */
/*--------------------------*/

TEST(HashTableTest, InsertSearch1) {
    std::vector<double> vec{1, 2, 3};
    Point a(vec, "point a");
    HashTable ht(10);
    ht.insert(0, 1, &a);

    ASSERT_EQ(ht.search(0, 1), 1);
}

TEST(HashTableTest, InsertSearch2) {
    std::vector<double> vec{1, 2, 3};
    Point a(vec, "point a");
    HashTable ht(10);
    ht.insert(0, 1, &a);

    ASSERT_EQ(ht.search(0, 2), 0);
}

TEST(HashTableTest, Remove) {
    std::vector<double> vec{1, 2, 3};
    Point a(vec, "point a");
    HashTable ht(10);
    ht.remove(0, 1);

    ASSERT_EQ(ht.search(0, 1), 0);
}

TEST(HashTableTest, GetBucket) {
    std::vector<double> vec{ 10, 20, 30 };
    Point a(vec, "point a");
    HashTable ht(10);
    ht.insert(0, 1, &a);

    Point b(vec, "point b");
    ht.insert(0, 2, &b);

    ASSERT_EQ(ht.getBucket(0).size(), 2);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}