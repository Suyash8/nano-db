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

TEST(TimestampCompressor, CompressesVaryingDeltas) {
    BitWriter writer;
    TimestampCompressor ts_comp(writer);

    ts_comp.addTimestamp(1000);
    ts_comp.addTimestamp(1060);
    ts_comp.addTimestamp(1125);
    ts_comp.addTimestamp(1130);
    
    // Bit Calculation:
    // First timestamp: 64 bits
    // Second timestamp: delta = 60, dod = 60 -> 2 bits (0b10) + 7 bits = 9 bits
    // Third timestamp: delta = 65, dod = 5 -> 2 bits (0b10) + 7 bits = 9 bits
    // Fourth timestamp: delta = 5, dod = -60 -> 2 bits (0b10) + 7 bits = 9 bits
    // Total = 64 + 9 + 9 + 9 = 91 bits = 12 bytes (11.375 rounded up)

    writer.flush();

    EXPECT_EQ(writer.getData().size(), 12);
}