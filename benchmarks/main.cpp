#include <chrono>
#include <iostream>

#include "nanodb/nanodb.hpp"

int main() {
    NanoDB db(1000);
    int count = 1000000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < count; ++i)
        db.insert(i, static_cast<double>(i));

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = end - start;
    double ops_per_sec = count / diff.count();

    std::cout << "Write speed: " << ops_per_sec << " ops/sec" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    db.query(0, 1000000);
    end = std::chrono::high_resolution_clock::now();

    diff = end - start;
    ops_per_sec = count / diff.count();

    std::cout << "Read speed: " << ops_per_sec << " ops/sec" << std::endl;
}