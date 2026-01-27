#pragma once

#include <bit>
#include <cstdint>
#include <stdexcept>

#include "bit_writer.hpp"
#include "utils.hpp"

class ValueCompressor {
    BitWriter& writer_;
    uint64_t prev_bits_;
    int prev_leading_;
    int prev_trailing_;
    bool first_;

public:
    ValueCompressor(BitWriter& writer) : writer_(writer), prev_bits_(0), prev_leading_(0x3F), prev_trailing_(0), first_(true) {}

    void addValue(double value) {
        uint64_t val_bits = std::bit_cast<uint64_t>(value);

        if (first_) {
            writer_.writeBits(val_bits, 64);
            prev_bits_ = val_bits;
            first_ = false;
            return;
        }

        uint64_t xor_val = val_bits ^ prev_bits_;

        if (xor_val == 0) {
            writer_.writeBit(0);
        } else {
            int leading = nanodb::countLeadingZeros(xor_val);
            int trailing = nanodb::countTrailingZeros(xor_val);

            if (leading >= prev_leading_ && trailing >= prev_trailing_) {
                writer_.writeBits(0b10, 2);
                int len = 64 - prev_leading_ - prev_trailing_;

                writer_.writeBits(xor_val >> prev_trailing_, len);
            } else {
                writer_.writeBits(0b11, 2);
                writer_.writeBits(leading, 5);
                int len = 64 - leading - trailing;
                writer_.writeBits(len, 6);

                writer_.writeBits(xor_val >> trailing, len);
                prev_leading_ = leading;
                prev_trailing_ = trailing;
            }
        }

        prev_bits_ = val_bits;
    }
};