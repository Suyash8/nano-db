#pragma once

#include <bit>
#include <cstdint>

#include "bit_reader.hpp"

class ValueDecompressor {
    BitReader& reader_;
    uint64_t prev_bits_;
    int prev_leading_;
    int prev_trailing_;
    bool first_;

public:
    ValueDecompressor(BitReader& reader) : reader_(reader), prev_bits_(0), prev_leading_(0), prev_trailing_(0), first_(true) {}

    double next() {
        if (first_) {
            uint64_t val_bits = reader_.readBits(64);
            prev_bits_ = val_bits;
            first_ = false;
            return std::bit_cast<double>(val_bits);
        }

        if (reader_.readBit() == 0) return std::bit_cast<double>(prev_bits_);

        uint64_t xor_val;
        if (reader_.readBit() == 0) {
            int len = 64 - prev_leading_ - prev_trailing_;
            uint64_t meaningful_bits = reader_.readBits(len);
            xor_val = meaningful_bits << prev_trailing_;
        } else {
            int leading = reader_.readBits(5);
            int len = reader_.readBits(6);
            int trailing = 64 - leading - len;

            uint64_t meaningful_bits = reader_.readBits(len);

            xor_val = meaningful_bits << trailing;

            prev_leading_ = leading;
            prev_trailing_ = trailing;
        }

        uint64_t current_bits = prev_bits_ ^ xor_val;
        prev_bits_ = current_bits;
        return std::bit_cast<double>(current_bits);
    }
};