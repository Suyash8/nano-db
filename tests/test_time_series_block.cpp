#include <gtest/gtest.h>

#include "nanodb/bit_writer.hpp"
#include "nanodb/time_series_block.hpp"

TEST(TimeSeriesBlock, StoresDataAndTracksMetadata) {
    TimeSeriesBlock block;

    block.append(1000, 10.5);
    block.append(1060, 10.6);
    block.append(1120, 10.7);

    ASSERT_EQ(block.getStartTime(), 1000);
    ASSERT_EQ(block.getEndTime(), 1120);
    ASSERT_EQ(block.getCount(), 3);
    ASSERT_NEAR(block.getSize(), 33, 3);

    block.close();
    ASSERT_NO_THROW(block.getData());
};

TEST(TimeSeriesBlock, CanReadBackData) {
    TimeSeriesBlock block;

    block.append(1000, 10.0);
    block.append(1060, 11.0);
    block.append(1120, 12.0);

    auto it = block.getIterator();

    int64_t ts;
    double val;

    // First Point
    ASSERT_TRUE(it.next(ts, val));
    EXPECT_EQ(ts, 1000);
    EXPECT_DOUBLE_EQ(val, 10.0);

    // Second Point
    ASSERT_TRUE(it.next(ts, val));
    EXPECT_EQ(ts, 1060);
    EXPECT_DOUBLE_EQ(val, 11.0);

    // Third Point
    ASSERT_TRUE(it.next(ts, val));
    EXPECT_EQ(ts, 1120);
    EXPECT_DOUBLE_EQ(val, 12.0);

    // End
    ASSERT_FALSE(it.next(ts, val));
};