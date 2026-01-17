#include <vector>
#include <cstdint>

class BitWriter {
    std::vector<uint8_t> buffer_;
    uint8_t scratch_;
    int bit_count_;

public:
    BitWriter() : scratch_(0), bit_count_(0) {}
    void writeBit(bool bit) {
        if (bit) scratch_ |= (1 << (7 - bit_count_));
        bit_count_++;

        if (bit_count_ == 8) {
            buffer_.push_back(scratch_);
            scratch_ = 0;
            bit_count_ = 0;
        }
    }

    void writeBits(uint64_t value, int num_bits) {
        for (int i = num_bits - 1; i >= 0; --i)
            writeBit((value >> i) & 1);
    }

    void flush() {
        if (bit_count_ > 0) {
            buffer_.push_back(scratch_);
            scratch_ = 0;
            bit_count_ = 0;
        }
    }

    const std::vector<uint8_t>& getData() const {
        return buffer_;
    }
};