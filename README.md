# Low Latency Order Matching Engine

## Overview

A real-time, multi-threaded order matching engine built in C++ that streams
live market data from Binance over WebSocket and matches orders with a median
latency of ~400-600ns. Built from scratch to understand how real trading
systems work at a systems-engineering level — focusing on low latency,
concurrency, correctness, and honest performance analysis.

## Tech Stack

- Language: C++17
- Compiler: GCC 16+ (MSYS2)
- Market Data: Binance WebSocket API (real-time streaming) + REST API
- Networking: Boost.Beast, Boost.Asio, OpenSSL
- JSON Parsing: nlohmann/json
- Concurrency: std::thread, std::atomic (lock-free)

## Architecture

| Component      | Responsibility                                                                       |
| -------------- | ------------------------------------------------------------------------------------ |
| MarketDataFeed | Connects to Binance WebSocket, parses JSON order book updates, creates Order objects |
| RingBuffer     | Lock-free SPSC buffer decoupling the receiver thread from the matching thread        |
| Exchange       | Validates orders, routes to correct OrderBook and MatchingEngine per symbol          |
| OrderBook      | Stores resting orders using a price-level array, maintains best bid/ask in O(1)      |
| MatchingEngine | Matches buyers and sellers, handles partial fills, LIMIT and MARKET orders           |
| Logger         | Timestamped file-based logging for trades and errors                                 |
| LatencyTracker | Tracks per-order latency and computes p50/p90/p99 percentiles                        |

## Key Design Decisions

### 1. Price Level Array instead of std::map

Replaced `std::map` with `std::vector<std::deque<Order>>` indexed directly by price.
Map operations are O(log n) due to red-black tree traversal and pointer chasing.
The array gives O(1) direct index access and is cache-friendly due to contiguous memory.
This single change reduced latency from 997ns to 294ns (3.4x improvement).

### 2. Lazy Deletion for Cancelled Orders

Instead of removing cancelled orders from the deque immediately (O(n)),
cancelled order IDs are stored in an `unordered_set`.
The matching engine checks this set before executing any order — an O(1) check.
Background cleanup removes stale orders from the deque to prevent memory growth.

### 3. Integer Price Representation

Prices are stored as integers (smallest currency unit) instead of floats.
Float arithmetic causes precision errors — a value like 3500.10 becomes
3500.099999999 internally. Integer representation guarantees exact math
with zero rounding errors, which is critical for financial correctness.

### 4. Per-Symbol Dynamic OrderBook

Each trading symbol gets its own OrderBook and MatchingEngine instance,
managed via `unique_ptr` in the Exchange layer and routed by an `unordered_map`.
Price ranges are configured per symbol to control the array's memory footprint.

### 5. Lock-Free SPSC Ring Buffer

A single-producer-single-consumer ring buffer decouples the WebSocket receiver
from the matching engine, allowing both to run on separate threads without
blocking each other. Head and tail indices use `std::atomic` with deliberate
memory orderings — `relaxed` for a thread's own index, `acquire`/`release` for
cross-thread visibility — achieving thread safety **without locks or mutexes**.
The buffer size is a power of two, enabling fast index wrapping via bitwise AND
(`index & (SIZE-1)`) instead of the modulo operator.

## Performance

### Core Matching Latency (benchmarked, 100,000 orders, -O2)

| Version                  | Without -O2 | With -O2 |
| ------------------------ | ----------- | -------- |
| std::map implementation  | 1971ns      | 997ns    |
| Price array optimization | 816ns       | 294ns    |
| With Exchange layer      | -           | 330ns    |

### Live Latency Percentiles (real Binance WebSocket data)

| Percentile | Latency           |
| ---------- | ----------------- |
| p50        | ~400-600ns        |
| p90        | ~4,000-9,000ns    |
| p99        | ~11,000-138,000ns |

**Analysis of the p99 tail:** The large gap between p50 and p99 is not a bug —
it correlates directly with order size. A large order that matches against many
resting orders in a single `processOrder()` call (a "fill cascade") must iterate
through each partial fill, taking proportionally longer than a simple order.

**Why order-splitting doesn't fix it:** Splitting a large order into smaller chunks
doesn't reduce total matching work. Because the ring buffer follows strict FIFO
ordering, all chunks of a split order are still processed sequentially before the
next distinct order in the queue. Meaningfully bounding worst-case latency would
require a priority-queue scheduler or multiple parallel matching queues — noted as
future work rather than a naive fix.

## Build and Run

### Requirements

- GCC 16+ (MSYS2 recommended on Windows)
- Boost and OpenSSL (`pacman -S mingw-w64-x86_64-boost mingw-w64-x86_64-openssl`)
- Internet connection (for Binance WebSocket)

### Compile

```bash
g++ src/main.cpp src/orderbook.cpp src/matchingengine.cpp src/exchange.cpp src/marketdata.cpp src/logger.cpp src/ringbuffer.cpp src/latencytracker.cpp -O2 -std=c++17 -lws2_32 -lwsock32 -lssl -lcrypto -o engine
```

### Run

```bash
./engine
```

Connects to the Binance BTC/USDT WebSocket stream, feeds live order book updates
through the ring buffer into the matching engine, and logs executed trades to
`trades.log` while printing latency percentiles periodically.

## Future Improvements

- Custom memory allocator to eliminate malloc from the hot path
- Priority-queue scheduler to bound worst-case (p99) latency for large orders
- Concurrent multi-symbol WebSocket streaming
- Async logging to fully remove file I/O from the matching path
- Graceful shutdown (clean thread joins, WebSocket close, log flush)
