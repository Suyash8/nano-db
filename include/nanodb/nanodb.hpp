#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include "block_iterator.hpp"
#include "time_series_block.hpp"

class NanoDB {
    std::vector<TimeSeriesBlock> blocks_;
    size_t max_block_size_;

public:
    NanoDB(size_t max_block_size = 1000) : max_block_size_(max_block_size) {}

    void insert(int64_t ts, double val) {
        if (blocks_.empty() || blocks_.back().getCount() >= max_block_size_) {
            if (!blocks_.empty()) blocks_.back().close();
            blocks_.emplace_back();
        }

        blocks_.back().append(ts, val);
    }

    std::vector<std::pair<int64_t, double>> query(int64_t start, int64_t end) {
        std::vector<std::pair<int64_t, double>> result;

        for (auto &block : blocks_) {
            if (block.getEndTime() < start) continue;
            if (block.getStartTime() > end) break;

            BlockIterator it = block.getIterator();
            
            int64_t ts;
            double val;

            while (it.next(ts, val)) {
                if (ts > end) break;
                if (ts >= start) result.emplace_back(ts, val);
            }
        }

        return result;
    }

    size_t getBlockCount() { return blocks_.size(); }
};