#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "bit_reader.hpp"
#include "timestamp_decompressor.hpp"
#include "value_decompressor.hpp"

class BlockIterator {
    BitReader reader_;
    TimestampDecompressor ts_dcomp_;
    ValueDecompressor val_dcomp_;
    size_t count_;
    size_t current_index_;

public:
    BlockIterator(const std::vector<uint8_t>& data, size_t count)
        : reader_(data), ts_dcomp_(reader_), val_dcomp_(reader_), count_(count), current_index_(0) {}

    bool next(int64_t& out_ts, double& out_val) {
        if (current_index_ >= count_) return false;

        out_ts = ts_dcomp_.next();
        out_val = val_dcomp_.next();

        current_index_++;
        return true;
    }
};