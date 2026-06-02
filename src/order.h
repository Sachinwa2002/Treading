#pragma once
#include <string>
#include <cstdint>

enum class Side {
    BUY,
    SELL
};

enum class OrderType {
    LIMIT,
    MARKET
};

enum class Status {
    NEW,
    PARTIAL_FILL,
    FILLED,
    CANCELLED
};

struct Order {
    Side side;
    std::string symbol;
    int64_t price;
    uint64_t quantity;
    OrderType orderType;
    uint64_t orderId;
    uint64_t clientId;
    uint64_t timeStamp;
    uint64_t remainingQuantity;
    Status status;
};

struct OrderLocation {
    int64_t price;
    Side side;
};