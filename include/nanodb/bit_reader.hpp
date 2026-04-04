#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

class BitReader {
    const std::vector<uint8_t>& buffer_;
    size_t byte_index_;
    int bit_index_;

public:
    BitReader(const std::vector<uint8_t>& buffer)
        : buffer_(buffer), byte_index_(0), bit_index_(0) {}

    bool readBit() {
        if (byte_index_ >= buffer_.size()) throw std::out_of_range("No more bits to read");
        const uint8_t& byte = buffer_[byte_index_];
        bool bit = (byte >> (7 - bit_index_)) & 1;

        ++bit_index_;
        if (bit_index_ > 7) {
            bit_index_ = 0;
            ++byte_index_;
        }

        return bit;
    }

    uint64_t readBits(int num_bits) {
        uint64_t result = 0;

        for (int i = 0; i < num_bits; ++i) {
            result <<= 1;
            if (readBit()) result |= 1;
        }

        return result;
    }
};