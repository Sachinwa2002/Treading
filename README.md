# Low Latency Order Matching Engine

## Overview

A real-time order matching engine built in C++ that fetches live market data
from Binance REST API and executes trades in under 300 nanoseconds.
Built to understand how real trading systems work at a systems engineering level —
focusing on low latency, correctness, and clean architecture.

## Tech Stack

- Language: C++17
- Compiler: GCC 16+ (MSYS2)
- Market Data: Binance REST API
- JSON Parsing: nlohmann/json

## Architecture

MarketDataFeed → Exchange → OrderBook → MatchingEngine → executeTrade

| Component      | Responsibility                                                                   |
| -------------- | -------------------------------------------------------------------------------- |
| MarketDataFeed | Fetches real-time data from Binance REST API, parses JSON, creates Order objects |
| Exchange       | Validates orders, routes to correct OrderBook and MatchingEngine per symbol      |
| OrderBook      | Stores resting orders using price level array, maintains best bid/ask            |
| MatchingEngine | Matches buyers and sellers, handles partial fills, LIMIT and MARKET orders       |

## Key Design Decisions

### 1. Price Level Array instead of std::map

Replaced `std::map` with `std::vector<std::deque<Order>>` indexed directly by price.
Map operations are O(log n) due to red-black tree traversal and pointer chasing.
Array gives O(1) direct index access and is cache-friendly due to contiguous memory.
This single change reduced latency from 997ns to 294ns (3.4x improvement).

### 2. Lazy Deletion for Cancelled Orders

Instead of removing cancelled orders from the deque immediately (O(n)),
cancelled order IDs are stored in an `unordered_set`.
The matching engine checks this set before executing any order — O(1) check.
Background cleanup removes cancelled orders from the deque to prevent memory growth.

### 3. Integer Price Representation (Paise)

Prices are stored as integers (paise) instead of floats.
Float arithmetic causes precision errors — ₹3500.10 becomes 3500.099999999 internally.
Integer representation guarantees exact math with zero rounding errors.

### 4. Per-Symbol Dynamic OrderBook

Each trading symbol gets its own OrderBook and MatchingEngine instance.
The Exchange layer routes orders by symbol using an unordered_map.
Price ranges are configured per symbol to control memory usage.

## Performance Benchmarks

| Version                  | Without -O2 | With -O2 |
| ------------------------ | ----------- | -------- |
| std::map implementation  | 1971ns      | 997ns    |
| Price array optimization | 816ns       | 294ns    |
| With Exchange layer      | -           | 330ns    |

Tested with 100,000 orders on single symbol (TCS).
Benchmarked on Windows with GCC 16, -O2 optimization flag.

## Build and Run

### Requirements

- GCC 16+ (MSYS2 recommended on Windows)
- Internet connection (for Binance API)

### Compile

```bash
g++ src/main.cpp src/orderbook.cpp src/matchingengine.cpp src/exchange.cpp src/marketdata.cpp -O2 -std=c++17 -o engine
```

### Run

```bash
./engine
```

Fetches real BTC/USDT order book from Binance every 5 seconds and processes live trades.

## Future Improvements

- WebSocket integration for true real-time streaming (replacing REST polling)
- Lock-free data structures using `std::atomic` for thread safety
- Custom memory allocator to eliminate malloc from hot path
- Support for more symbols and asset classes
- Async logging to remove I/O from critical matching path
- p99 latency measurement and reporting
