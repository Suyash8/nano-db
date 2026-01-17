#include <gtest/gtest.h>
#include "nanodb/simple_time_series.hpp"

TEST(SimpleTimeSeries, CanAddAndCheckSize) {
    SimpleTimeSeries simpleTimeSeries;
    simpleTimeSeries.add(1000, 10.5);
    simpleTimeSeries.add(1001, 10.6);

    ASSERT_EQ(simpleTimeSeries.size(), 2);
}