#pragma once

#include <cstdint>

#include "bit_reader.hpp"

class TimestampDecompressor {
    BitReader& reader_;
    int64_t prev_timestamp_;
    int64_t prev_delta_;
    bool first_;

public:
    TimestampDecompressor(BitReader& reader) : reader_(reader), prev_timestamp_(0), prev_delta_(0), first_(true) {}

    int64_t next() {
        if (first_) {
            int64_t timestamp = reader_.readBits(64);
            prev_timestamp_ = timestamp;
            prev_delta_ = 0;
            first_ = false;
            return timestamp;
        }

        uint8_t bits = 0;

        if (reader_.readBit() == 0) bits = 0;
        else if (reader_.readBit() == 0) bits = 7;
        else if (reader_.readBit() == 0) bits = 9;
        else if (reader_.readBit() == 0) bits = 12;
        else bits = 32;

        int64_t dod = reader_.readBits(bits);

        if (bits > 0) {
            int64_t max_positive = (1ULL << (bits - 1));
            if (dod >= max_positive) dod -= (1ULL << bits);
        }

        prev_delta_ += dod;
        prev_timestamp_ += prev_delta_;

        return prev_timestamp_;
    }
};