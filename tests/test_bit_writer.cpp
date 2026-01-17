#include <gtest/gtest.h>

#include "nanodb/bit_writer.hpp"

TEST(BitWriter, WriteSingleBits) {
    BitWriter writer;
    writer.writeBit(1);
    writer.writeBit(0);
    writer.writeBit(1);
    writer.writeBit(1);
    writer.writeBit(0);
    writer.writeBit(0);
    writer.writeBit(1);
    writer.writeBit(0);

    const auto& data = writer.getData();
    ASSERT_EQ(data.size(), 1);
    EXPECT_EQ(data[0], 0xB2);
}

TEST(BitWriter, CanWriteMultiBits) {
    BitWriter writer;
    writer.writeBits(0b10110, 5);
    writer.writeBits(0b111, 3);

    const auto& data = writer.getData();
    ASSERT_EQ(data.size(), 1);
    EXPECT_EQ(data[0], 0xB7);
}

TEST(BitWriter, FlushWritesPartialByte) {
    BitWriter writer;
    writer.writeBits(0b101, 3);
    writer.flush();

    const auto& data = writer.getData();
    ASSERT_EQ(data.size(), 1);
    EXPECT_EQ(data[0], 0b10100000);
}