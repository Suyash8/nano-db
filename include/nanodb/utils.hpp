#pragma once

#include <bit>
#include <cstdint>

namespace nanodb {
    inline int countLeadingZeros(uint64_t val) {
        return std::countl_zero(val);
    }

    inline int countTrailingZeros(uint64_t val) {
        return std::countr_zero(val);
    }
}