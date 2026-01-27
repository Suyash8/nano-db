#include <gtest/gtest.h>

#include "nanodb/bit_reader.hpp"
#include "nanodb/bit_writer.hpp"
#include "nanodb/value_compressor.hpp"
#include "nanodb/value_decompressor.hpp"

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

TEST(ValueCompressor, RoundTrip) {
    std::vector<double> values = {3.14, 3.1415, 3.14159, 2.71828, 2.7182818, 1.61803, 1.6180339};

    BitWriter writer;
    ValueCompressor val_comp(writer);

    for (double val : values) {
        val_comp.addValue(val);
    }

    writer.flush();
    std::vector<uint8_t> compressed_data = writer.getData();

    BitReader reader(compressed_data);
    ValueDecompressor val_decomp(reader);

    for (double expected_val : values) {
        double decompressed_val = val_decomp.next();
        EXPECT_DOUBLE_EQ(decompressed_val, expected_val);
    }
}