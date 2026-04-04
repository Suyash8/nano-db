#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "block_iterator.hpp"
#include "time_series_block.hpp"

class NanoDB {
    std::vector<TimeSeriesBlock> blocks_;
    size_t max_block_size_;
    mutable std::shared_mutex rw_lock_;

public:
    NanoDB(size_t max_block_size = 1000) : max_block_size_(max_block_size) {}

    void insert(int64_t ts, double val) {
        std::unique_lock<std::shared_mutex> lock(rw_lock_);

        if (blocks_.empty() || blocks_.back().getCount() >= max_block_size_) {
            if (!blocks_.empty()) blocks_.back().close();
            blocks_.emplace_back();
        }

        blocks_.back().append(ts, val);
    }

    std::vector<std::pair<int64_t, double>> query(int64_t start, int64_t end) {
        std::shared_lock<std::shared_mutex> lock(rw_lock_);

        std::vector<std::pair<int64_t, double>> result;

        auto it = std::lower_bound(blocks_.begin(), blocks_.end(), start,
            [](TimeSeriesBlock& block, int64_t val) {
                return block.getEndTime() < val;
            });

        for (; it != blocks_.end(); ++it) {
            auto& block = *it;

            if (block.getStartTime() > end) break;

            BlockIterator block_it = block.getIterator();

            int64_t ts;
            double val;

            while (block_it.next(ts, val)) {
                if (ts > end) break;
                if (ts >= start) result.emplace_back(ts, val);
            }
        }

        return result;
    }

    void save(std::string& filename) {
        std::unique_lock<std::shared_mutex> lock(rw_lock_);

        if (!blocks_.empty() && blocks_.back().getCount() < max_block_size_) blocks_.back().close();

        std::ofstream file(filename, std::ios::binary);

        file.write("NANO", 4);

        size_t size = blocks_.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));

        for (auto& block : blocks_) {
            int64_t start_time = block.getStartTime();
            file.write(reinterpret_cast<const char*>(&start_time), sizeof(start_time));

            int64_t end_time = block.getEndTime();
            file.write(reinterpret_cast<const char*>(&end_time), sizeof(end_time));

            int64_t count = block.getCount();
            file.write(reinterpret_cast<const char*>(&count), sizeof(count));

            int64_t data_size = block.getSize();
            file.write(reinterpret_cast<const char*>(&data_size), sizeof(data_size));

            file.write(reinterpret_cast<const char*>(block.getData().data()), data_size);
        }
    }

    void load(std::string& filename) {
        std::unique_lock<std::shared_mutex> lock(rw_lock_);

        std::ifstream file(filename, std::ios::binary);

        if (!file.is_open()) throw std::runtime_error("Could not open file");

        char magic[4];
        file.read(magic, 4);
        if (std::string(magic, 4) != "NANO") throw std::runtime_error("Invalid file format");

        size_t count;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));
        blocks_.clear();

        for (size_t i = 0; i < count; ++i) {
            int64_t start_time, end_time, point_count, data_size;
            file.read(reinterpret_cast<char*>(&start_time), sizeof(start_time));
            file.read(reinterpret_cast<char*>(&end_time), sizeof(end_time));
            file.read(reinterpret_cast<char*>(&point_count), sizeof(point_count));
            file.read(reinterpret_cast<char*>(&data_size), sizeof(data_size));

            std::vector<uint8_t> data(data_size);
            file.read(reinterpret_cast<char*>(data.data()), data_size);

            blocks_.emplace_back(start_time, end_time, point_count, data);
        }
    }

    size_t getBlockCount() {
        std::shared_lock<std::shared_mutex> lock(rw_lock_);
        return blocks_.size();
    }
};