#include <gtest/gtest.h>
#include <iostream>
#include "nanodb/simple_time_series.hpp"

TEST(SimpleTimeSeries, CanAddAndCheckSize) {
    SimpleTimeSeries ts;
    ts.add(1000, 10.5);
    ts.add(1001, 10.6);

    ASSERT_EQ(ts.size(), 2);
}

TEST(SimpleTimeSeries, Benchmark1MillionPoints) {
    SimpleTimeSeries ts;

    for (int i = 0; i < 1000000; ++i)
        ts.add(i, 100.0 + (i % 100));

    ASSERT_EQ(ts.size(), 1000000);

    double mb = ts.bytes() / (1024.0 * 1024.0);
    std::cout << "Naive Implementation Memory: " << mb << " MB" << std::endl;
}