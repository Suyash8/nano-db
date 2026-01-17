#include <gtest/gtest.h>

#include "nanodb/bit_writer.hpp"
#include "nanodb/bit_reader.hpp"

TEST(BitReader, ReadSingleBits) {
    std::vector<uint8_t> data = {0b10110010};
    BitReader reader(data);

    EXPECT_EQ(reader.readBit(), 1);
    EXPECT_EQ(reader.readBit(), 0);
    EXPECT_EQ(reader.readBit(), 1);
    EXPECT_EQ(reader.readBit(), 1);
    EXPECT_EQ(reader.readBit(), 0);
    EXPECT_EQ(reader.readBit(), 0);
    EXPECT_EQ(reader.readBit(), 1);
    EXPECT_EQ(reader.readBit(), 0);
}

TEST(BitReader, CanReadMultiBits) {
    std::vector<uint8_t> data = {0b10110111};
    BitReader reader(data);

    EXPECT_EQ(reader.readBits(5), 0b10110);
    EXPECT_EQ(reader.readBits(3), 0b111);
}

TEST(BitReader, RoundTrip) {
    BitWriter writer;
    writer.writeBits(0b10110, 5);
    writer.writeBits(0b111, 3);

    const auto& bytes = writer.getData();
    BitReader reader(bytes);

    EXPECT_EQ(reader.readBits(5), 0b10110);
    EXPECT_EQ(reader.readBits(3), 0b111);
}