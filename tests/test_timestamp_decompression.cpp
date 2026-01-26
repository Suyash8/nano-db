#include <gtest/gtest.h>

#include "nanodb/bit_writer.hpp"
#include "nanodb/bit_reader.hpp"
#include "nanodb/timestamp_compressor.hpp"
#include "nanodb/timestamp_decompressor.hpp"

TEST(TimestampCompressor, RoundTripDecompression) {
    BitWriter writer;
    TimestampCompressor ts_comp(writer);

    ts_comp.addTimestamp(1000);
    ts_comp.addTimestamp(1060);
    ts_comp.addTimestamp(1125);
    ts_comp.addTimestamp(1120);
    ts_comp.addTimestamp(1120);

    writer.flush();

    BitReader reader(writer.getData());
    TimestampDecompressor ts_decomp(reader);

    EXPECT_EQ(ts_decomp.next(), 1000);
    EXPECT_EQ(ts_decomp.next(), 1060);
    EXPECT_EQ(ts_decomp.next(), 1125);
    EXPECT_EQ(ts_decomp.next(), 1120);
    EXPECT_EQ(ts_decomp.next(), 1120);
}