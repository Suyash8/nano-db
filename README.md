# NanoDB

<p align="center">
  <img src="https://img.shields.io/badge/C++-20-blue?style=for-the-badge&logo=c%2B%2B" />
  <img src="https://img.shields.io/badge/CMake-Build-green?style=for-the-badge&logo=cmake" />
  <img src="https://img.shields.io/badge/GoogleTest-Unit%20Tests-red?style=for-the-badge&logo=google" />
  <img src="https://img.shields.io/badge/Database-Time--Series-grey?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Compression-Bit--Packed-lightgrey?style=for-the-badge" />
</p>

<!-- <p align="center">
  <img src="https://img.shields.io/github/languages/top/Suyash8/nano-db" />
  <img src="https://img.shields.io/github/stars/Suyash8/nano-db" />
  <img src="https://img.shields.io/github/last-commit/Suyash8/nano-db" />
</p> -->

NanoDB is a small time-series database written in C++20.
It stores timestamp/value points in compressed blocks and serves them through a CLI shell or a TCP server.
The project exists to test practical time-series compression and query performance in a simple codebase.

## Overview

The database accepts points as `(timestamp, double value)` pairs.
Data is appended to in-memory blocks, compressed bit-by-bit, and queried by time range.

This project is intentionally narrow in scope:
- One data model (time-series points)
- One main query type (range query)
- One file format for save/load

The goal is clarity and speed in the hot path, not feature completeness.

## Architecture / Design

At a high level, the system has three layers:

1. Storage and compression layer
- `BitWriter` and `BitReader` handle packed bit I/O.
- `TimestampCompressor` / `TimestampDecompressor` implement delta-of-delta encoding.
- `ValueCompressor` / `ValueDecompressor` implement Gorilla-style XOR encoding for doubles.

2. Block layer
- `TimeSeriesBlock` owns compressed bytes plus metadata (`start_time`, `end_time`, count).
- `BlockIterator` streams decompressed points from a block.

3. Database and interfaces
- `NanoDB` manages a vector of blocks and supports insert/query/save/load.
- `CommandDispatcher` maps text commands to database operations.
- `nanodb_shell` provides local interactive access.
- `nanodb_server` exposes the same commands over TCP.

Concurrency model:
- `NanoDB` uses `std::shared_mutex`.
- Writes take an exclusive lock.
- Queries take a shared lock.

## Key Features

- Bit-packed storage for compressed block payloads.
- Timestamp compression with delta-of-delta encoding.
- Floating-point compression using XOR and meaningful-bit windows.
- Block-level metadata to skip irrelevant blocks during range queries.
- Binary save/load format (`NANO` magic + per-block payloads).
- Command-based shell and TCP server using the same dispatcher.
- Unit tests for bit I/O, compressors/decompressors, blocks, and database behavior.

## How It Works

### Insert path

1. `insert(ts, value)` acquires a write lock.
2. If the current block is full, it is closed and a new block is created.
3. Timestamp and value are encoded into the active block bitstream.
4. Block metadata is updated (`start_time`, `end_time`, count).

### Query path

1. `query(start, end)` acquires a shared lock.
2. It uses `std::lower_bound` on block metadata to find the first potentially relevant block.
3. Each relevant block is decompressed through `BlockIterator`.
4. Points are filtered by time range and returned.

### Save/load

- Save writes:
  - 4-byte magic (`NANO`)
  - block count
  - per-block metadata and compressed byte payload
- Load validates the magic header, clears existing blocks, then reconstructs blocks from file data.

### Compression choices and tradeoffs

- **Delta-of-delta**: Works exceptionally well for timestamps with regular sampling intervals.
- **Gorilla XOR Compression**: NanoDB leverages the Gorilla XOR compression algorithm to efficiently store double-precision floating-point values. Instead of storing the full 64-bit IEEE 754 representation for every data point, the engine calculates the XOR difference between the current value and the previous value. Since adjacent values in time-series data tend to be identical or very close, their XOR result often contains many leading and trailing zeros.

  When a value is XORed with the previous one, if the result is zero, NanoDB simply writes a single '0' bit. If the result is non-zero, it checks whether the meaningful bits (the non-zero portion) fall within the same bounds as the previous value. If they do, it only stores the meaningful bits. If the bounds change, it stores the new leading zero count, the length of the meaningful bits, and the bits themselves. This bit-packing strategy enables NanoDB to drastically reduce the storage footprint for high-frequency measurements.
- **In-memory design**: Data is kept in memory; this keeps code simple and fast for this project size, but it is not designed as a full persistent engine.

## Performance

Based on the built-in `nanodb_benchmark` tool, NanoDB demonstrates extremely high throughput for in-memory operations:
- **Write Speed**: ~37.0 million ops/sec
- **Read Speed**: ~36.2 million ops/sec

## Tech Stack

- Language: C++20
- Build system: CMake (3.15+)
- Testing: GoogleTest (via CMake FetchContent)
- Client example: Python 3 (socket-based)

## Setup & Running Instructions

### Prerequisites

- CMake 3.15+
- C++20 compiler (`g++` or `clang++`)
- Python 3 (optional, for client script)

### Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Run tests

```bash
ctest --test-dir build --output-on-failure
```

You can also run the test executable directly:

```bash
./build/nanodb_tests
```

### Run shell

```bash
./build/nanodb_shell
```

### Run TCP server

```bash
./build/nanodb_server
```

Default server port is `8080`.

### Run benchmark

```bash
./build/nanodb_benchmark
```

## Example Usage

### Shell commands

```text
insert 1700000000 101.25
insert 1700000060 101.40
query 1700000000 1700000100
stats
save test_db.nano
load test_db.nano
help
```

### Python client

```python
from nanodb_client import NanoDBClient

client = NanoDBClient(host="127.0.0.1", port=8080)
client.insert(1700000000, 101.25)
client.insert(1700000060, 101.40)

points = client.query(1700000000, 1700000100)
print(points)

client.close()
```

Script location:
- `clients/python/nanodb_client.py`
- `clients/python/test_client.py`

## Limitations / Future Work

Current limitations:
- In-memory design only; no background compaction or indexing beyond block metadata.
- Query decompression is block sequential once blocks are selected.
- Basic socket protocol (line-oriented text); no auth, TLS, or framing beyond newline handling.
- Error handling in server/client is minimal.

Planned improvements:
- Add stronger on-disk indexing for faster large-range scans.
- Improve network protocol (message framing, partial reads/writes, better validation).
- Add retention policies and block compaction.
- Add benchmarks with reproducible datasets and configuration presets.
- Expand tests around persistence edge cases and corrupted files.

## Notes

This project is a focused implementation, not a full production database.
The point is to keep the core compression and range-query pipeline easy to read, easy to test, and fast enough to experiment with.
