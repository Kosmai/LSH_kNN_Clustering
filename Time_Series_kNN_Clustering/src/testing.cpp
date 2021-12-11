#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <map>
#include "../inc/randGen.hpp"
#include "../inc/point.hpp"
#include "../inc/LSH.hpp"
#include "../inc/hypercube.hpp"
#include "../inc/searchUtils.hpp"
#include "../inc/timeSeries.hpp"
#include <gtest/gtest.h>

TEST(MyTest, SubTest1) {
    ASSERT_EQ(1, 2);
}

TEST(MyTest, SubTest2) {
    ASSERT_TRUE(1 == 1);
}

TEST(MyTest, SubTest3) {
    ASSERT_GT(2, 1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}