#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "bit_writer.hpp"
#include "timestamp_compressor.hpp"
#include "value_compressor.hpp"

class TimeSeriesBlock {
    BitWriter& writer_;
    TimestampCompressor ts_compressor_;
    ValueCompressor val_compressor_;
    int64_t start_time_;
    int64_t end_time_;
    size_t count_;
    bool closed_;

public:
    TimeSeriesBlock(BitWriter& writer) : writer_(writer), ts_compressor_(writer), val_compressor_(writer), start_time_(0), end_time_(0), count_(0), closed_(false) {}

    void append(int64_t ts, double value) {
        if (closed_) throw std::runtime_error("Cannot append to a closed TimeSeriesBlock");

        if (count_ == 0) start_time_ = ts;
        end_time_ = ts;
        ++count_;

        ts_compressor_.addTimestamp(ts);
        val_compressor_.addValue(value);
    }

    void close() {
        writer_.flush();
        closed_ = true;
    }

    int64_t getStartTime() { return start_time_; }
    int64_t getEndTime() { return end_time_; }
    size_t getCount() { return count_; }
    size_t getSize() { return writer_.getData().size(); }
    const std::vector<uint8_t>& getData() { return writer_.getData(); }
};