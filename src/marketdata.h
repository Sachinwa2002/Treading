#pragma once
#include <iostream>
#include "order.h"
#include "exchange.h"
#include "json.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

class MarketDataFeed{
    private:
    Exchange &exchange;
    uint64_t nextOrderId;
    std::string symbol;
    int processedCount = 0;

    // boost websocket objects
    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};
    std::unique_ptr<websocket::stream<beast::ssl_stream<beast::tcp_stream>>> ws;

    public:
    MarketDataFeed(Exchange &ex, std::string sym):
    exchange(ex),nextOrderId(1),symbol(sym){}

    void connectWebSocket();
    void runWebSocket();

    void fetchAndProcess();

    private:
    void processLevel(std::string price,std::string qty,Side side);
    void processMessage(const std::string& message);

};