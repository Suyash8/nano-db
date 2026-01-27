#include <gtest/gtest.h>

#include "nanodb/bit_writer.hpp"
#include "nanodb/value_compressor.hpp"

TEST(ValueCompressor, CompressesDuplicateValues) {
    BitWriter writer;
    ValueCompressor val_comp(writer);

    val_comp.addValue(100.50);
    val_comp.addValue(100.50);
    val_comp.addValue(100.50);

    writer.flush();

    EXPECT_EQ(writer.getData().size(), 9);  // 8 bytes for first value + 1 bit for each subsequent duplicate
}

TEST(ValueCompressor, CompressesChangingValues) {
    BitWriter writer;
    ValueCompressor val_comp(writer);

    val_comp.addValue(1.0);
    val_comp.addValue(1.0001);
    val_comp.addValue(1.0002);

    writer.flush();

    EXPECT_LT(writer.getData().size(), 24);  // Should be less than 3 full doubles (24 bytes)
}