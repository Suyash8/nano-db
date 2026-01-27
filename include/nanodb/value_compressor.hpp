#pragma once

#include <bit>
#include <cstdint>
#include <stdexcept>

#include "bit_writer.hpp"

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

        if (xor_val == 0) writer_.writeBit(0);
        else throw std::runtime_error("Non-zero XOR compression not implemented");

        prev_bits_ = val_bits;
    }
};