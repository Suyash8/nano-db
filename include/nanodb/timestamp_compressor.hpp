#include <stdexcept>

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

        if (dod == 0) writer_.writeBit(0);
        else throw std::runtime_error("Complex DOD not implemented yet");

        prev_timestamp_ = timestamp;
        prev_delta_ = delta;
    }
};