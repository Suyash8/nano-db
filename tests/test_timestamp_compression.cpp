#include <gtest/gtest.h>

#include "nanodb/timestamp_compressor.hpp"

TEST(TimestampCompressor, CompressesRegularIntervals) {
    BitWriter writer;
    TimestampCompressor ts_comp(writer);

    ts_comp.addTimestamp(1000);
    ts_comp.addTimestamp(1000);
    ts_comp.addTimestamp(1000);

    writer.flush();

    EXPECT_EQ(writer.getData().size(), 9);  // 8 bytes for first timestamp + 1 bit for each subsequent timestamp
}