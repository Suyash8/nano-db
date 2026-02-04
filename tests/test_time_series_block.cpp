#include <gtest/gtest.h>

#include "nanodb/bit_writer.hpp"
#include "nanodb/time_series_block.hpp"

TEST(TimeSeriesBlock, StoresDataAndTracksMetadata) {
    BitWriter writer;
    TimeSeriesBlock block(writer);

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