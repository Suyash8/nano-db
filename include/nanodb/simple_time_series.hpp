#pragma once

#include <vector>
#include <utility>
#include <cstdint>

class SimpleTimeSeries {
    std::vector<std::pair<int64_t, double>> data;

public:
    void add(int64_t timestamp, double value) {
        data.push_back({timestamp, value});
    }

    size_t size() const {
        return data.size();
    }
};