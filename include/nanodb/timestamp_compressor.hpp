#pragma once

#include <cstdint>

#include "bit_writer.hpp"

class TimestampCompressor {
    BitWriter& writer_;
    int64_t prev_timestamp_;
    int64_t prev_delta_;
    bool first_;

public:
    TimestampCompressor(BitWriter& writer) : writer_(writer), prev_timestamp_(0), prev_delta_(0), first_(true) {}

    void addTimestamp(int64_t timestamp) {
        if (first_) {
            writer_.writeBits(timestamp, 64);
            prev_timestamp_ = timestamp;
            first_ = false;
            return;
        }

        int64_t delta = timestamp - prev_timestamp_;
        int64_t dod = delta - prev_delta_;

        if (dod == 0) {
            writer_.writeBit(0);
        } else if (dod >= -63 && dod <= 64) {
            writer_.writeBits(0b10, 2);
            writer_.writeBits(dod, 7);
        } else if (dod >= -255 && dod <= 256) {
            writer_.writeBits(0b110, 3);
            writer_.writeBits(dod, 9);
        } else if (dod >= -2047 && dod <= 2048) {
            writer_.writeBits(0b1110, 4);
            writer_.writeBits(dod, 12);
        } else {
            writer_.writeBits(0b1111, 4);
            writer_.writeBits(dod, 32);
        }

        prev_timestamp_ = timestamp;
        prev_delta_ = delta;
    }
};