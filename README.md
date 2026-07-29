# Low Latency Order Matching Engine

A real-time, multi-threaded order matching engine built from scratch in C++.
It streams live order book data from Binance over WebSocket, matches orders
using price-time priority, and achieves a median (p50) matching latency of
~300-400 nanoseconds through a lock-free, cache-optimized architecture.

Built to deeply understand how real exchanges achieve low latency at a
systems-engineering level — with an emphasis on measurement, honest
performance analysis, and clean architecture over feature count.

---

## Highlights

- **~300-400ns p50 matching latency** on live market data
- **Lock-free SPSC ring buffer** connecting a producer (WebSocket) and
  consumer (matching engine) thread — no mutexes in the hot path
- **O(1) order book** using a price-level array instead of `std::map`
  (a measured 3.4x speedup)
- **Custom memory pool allocator** benchmarked at **2ns vs malloc's 91ns** (~45x)
- **Live Binance WebSocket feed** (SSL, real-time order book streaming)
- **Latency percentile tracking** (p50 / p90 / p99) with honest tail analysis

---

## Tech Stack

| Area         | Technology                              |
| ------------ | --------------------------------------- |
| Language     | C++17                                   |
| Compiler     | GCC 16+ (MSYS2 on Windows)              |
| Market Data  | Binance WebSocket API (+ REST fallback) |
| Networking   | Boost.Beast, Boost.Asio, OpenSSL        |
| JSON Parsing | nlohmann/json                           |
| Concurrency  | std::thread, std::atomic (lock-free)    |

---

## Architecture

PRODUCER THREAD CONSUMER THREAD
┌────────────────────────┐ ┌──────────────────────────┐
│ Binance WebSocket │ │ pop() from ring buffer │
│ → parse JSON │ Lock-Free SPSC │ → Exchange (validate, │
│ → build Order │ ═══ Ring Buffer ══>│ route by symbol) │
│ → push() to ring buffer│ │ → MatchingEngine │
└────────────────────────┘ │ → OrderBook │
└──────────────────────────┘

| Component      | Responsibility                                                                        |
| -------------- | ------------------------------------------------------------------------------------- |
| MarketDataFeed | Connects to Binance WebSocket, parses order book updates, pushes Orders to the buffer |
| RingBuffer     | Lock-free SPSC buffer decoupling the receiver thread from the matching thread         |
| Exchange       | Validates orders, routes to the correct OrderBook/MatchingEngine per symbol           |
| OrderBook      | Stores resting orders in a price-level array; O(1) best bid/ask access                |
| MatchingEngine | Matches buy/sell orders (price-time priority), partial fills, LIMIT & MARKET types    |
| MemoryPool     | O(1) fixed-size allocator to remove malloc from the hot path                          |
| Logger         | Timestamped, file-based trade and error logging                                       |
| LatencyTracker | Circular-buffer latency recorder computing p50 / p90 / p99                            |

---

## Key Design Decisions

### 1. Price-Level Array instead of std::map

Replaced `std::map<price, deque<Order>>` with `std::vector<std::deque<Order>>`
indexed directly by price. A `std::map` is a red-black tree — its nodes are
scattered across the heap, so each traversal is a pointer-chase that triggers a
cache miss (a ~100ns fetch from RAM). A contiguous array is cache-friendly:
accessing one price level pulls neighboring data into the same 64-byte cache
line, turning misses into hits. **Result: matching latency dropped from 997ns
to 294ns (3.4x).**

### 2. Lock-Free SPSC Ring Buffer

A single-producer-single-consumer ring buffer decouples the WebSocket receiver
from the matching engine so neither blocks the other. Head and tail indices are
`std::atomic` with deliberate memory orderings — `relaxed` for a thread's own
index, and **acquire/release** across threads. The release on the tail store
guarantees the order data is fully written and visible _before_ the tail update
becomes visible to the consumer — preventing the consumer from reading a slot
the producer hasn't finished writing. The buffer size is a power of two, so
index wrapping uses a bitwise AND (`i & (SIZE-1)`) instead of modulo.

### 3. Lazy Deletion for Cancelled Orders

Rather than removing a cancelled order from the middle of a deque (O(n)),
cancelled order IDs are inserted into an `unordered_set`. The matching engine
does an O(1) check against this set before executing any order, and stale
entries are cleaned from the deque in the background — keeping the hot path fast.

### 4. Integer Price Representation

Prices are stored as integers (smallest currency unit) rather than floats.
Float arithmetic introduces precision errors (e.g. 3500.10 stored as
3500.099999999) — unacceptable for financial correctness. Integers guarantee
exact math with zero rounding error.

### 5. Custom Memory Pool Allocator

A fixed-size pool pre-allocates one large block at startup and hands out slots
via an intrusive free list — O(1) allocate and deallocate, no system calls, no
locks. Benchmarked against `malloc`/`free` on Order-sized blocks:
**malloc ~91ns/op vs pool ~2ns/op (~45x faster)**, and far more _predictable_ —
directly relevant to reducing tail latency.

### 6. Per-Symbol OrderBook

Each symbol gets its own OrderBook and MatchingEngine (managed via `unique_ptr`
and routed through an `unordered_map`). Price ranges are configured per symbol
to bound the array's memory footprint.

---

## Performance

### Core Matching Latency (benchmark, 100,000 orders, `-O2`)

| Version                  | Without -O2 | With -O2 |
| ------------------------ | ----------- | -------- |
| std::map implementation  | 1971ns      | 997ns    |
| Price-array optimization | 816ns       | 294ns    |
| With Exchange layer      | –           | 330ns    |

### Memory Allocation (benchmark, 1,000,000 ops, `-O2`)

| Allocator   | Latency per op |
| ----------- | -------------- |
| malloc/free | ~91ns          |
| Memory pool | ~2ns           |

### Live Latency Percentiles (real Binance WebSocket data)

| Percentile | Latency          |
| ---------- | ---------------- |
| p50        | ~300-400ns       |
| p90        | ~3,000-9,000ns   |
| p99        | ~11,000ns and up |

**On the p99 tail:** The gap between p50 and p99 is not a bug — it correlates
directly with order size. A large order that matches against many resting orders
in a single `processOrder()` call (a "fill cascade") must iterate through each
partial fill, so its latency scales with fill count.

**Why order-splitting doesn't fix it:** Splitting a large order into chunks
doesn't reduce total matching work, and because the ring buffer is strictly FIFO,
all chunks are processed sequentially before the next distinct order — so the
blocking time is unchanged. Properly bounding worst-case latency would require a
priority-queue scheduler or parallel per-symbol matching queues (noted as future
work). A practical mitigation is a maximum order-size limit, which also doubles
as a risk control.

---

## Build and Run

### Requirements

- GCC 16+ (MSYS2 recommended on Windows)
- Boost and OpenSSL:

```bash
  pacman -S mingw-w64-x86_64-boost mingw-w64-x86_64-openssl
```

- Internet connection (for the Binance WebSocket feed)

### Compile

```bash
g++ src/main.cpp src/orderbook.cpp src/matchingengine.cpp src/exchange.cpp \
    src/marketdata.cpp src/logger.cpp src/ringbuffer.cpp src/latencytracker.cpp \
    -O2 -std=c++17 -lws2_32 -lwsock32 -lssl -lcrypto -o engine
```

### Run

```bash
./engine
```

Connects to the Binance BTC/USDT WebSocket stream, feeds live order book updates
through the ring buffer into the matching engine, logs executed trades to
`trades.log`, and prints latency percentiles periodically.

---

## Future Improvements

- **Profiling with `perf`** to locate the _measured_ hot path (in progress)
- **Priority-queue scheduler** to bound worst-case (p99) latency for large orders
- **Per-symbol threading** for true multi-symbol parallelism
- **Async logging** to fully remove file I/O from the matching path
- **Graceful shutdown** — clean thread joins, WebSocket close, log flush
- **Order-entry gateway** to accept external client orders (not just consume market data)
