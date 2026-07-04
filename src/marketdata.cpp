#include "json.hpp"
#include "marketdata.h"
#include <cstdio>
 #include <algorithm>
using json = nlohmann::json;

void MarketDataFeed::connectWebSocket(){
   std::string host="stream.binance.com";
   std::string port="9443";
   // convert symbol to lowercase for WebSocket target
   std::string lowerSymbol = symbol;
   std::transform(lowerSymbol.begin(), lowerSymbol.end(), 
                  lowerSymbol.begin(), ::tolower);
   std::string target = "/ws/" + lowerSymbol + "@depth5@100ms";

   //resolve hostname
   tcp::resolver resolver(ioc);
   auto const results = resolver.resolve(host,port);

   //TCP connect
   beast::tcp_stream stream(ioc);
   stream.connect(results);

   //SSL setup
   ctx.set_default_verify_paths();
   beast::ssl_stream<beast::tcp_stream>ssl_stream(std::move(stream),ctx);
   SSL_set_tlsext_host_name(ssl_stream.native_handle(),host.c_str());
   ssl_stream.handshake(ssl::stream_base::client);

   //websocket handshake
   ws = std::make_unique<websocket::stream<beast::ssl_stream<beast::tcp_stream>>>(std::move(ssl_stream));
   ws->handshake(host, target);

   std::cout<<"Connected to binance Websocket for symbol:" <<symbol<<std::endl;
}

void MarketDataFeed::runWebSocket(){
   beast::flat_buffer buffer;

   while(true){
      ws->read(buffer);
       std::cout << "Raw message received" << std::endl;
      std::string message=beast::buffers_to_string(buffer.data());
      processMessage(message);
      buffer.consume(buffer.size());
   }
}

void MarketDataFeed::processMessage(const std::string& message){
   std::cout << "Message received" << std::endl;
   json data=json::parse(message);

   for(auto &bid: data["bids"]){
      std::string price=bid[0];
      std::string qty = bid[1];
      processLevel(price,qty,Side::BUY);
   }

   for(auto& ask: data["asks"]){
      std::string price=ask[0];
      std::string qty=ask[1];
      processLevel(price,qty,Side::SELL);
   }
}

//this below was the REST API based implementation which we have replaced with WebSocket implementation
// void MarketDataFeed:: fetchAndProcess(){
//     //build URLS
//     std::string url="https://api.binance.com/api/v3/depth?symbol="+symbol+"&limit=5";

//     //build CURL command
//      std::string command = "curl -s \"" + url + "\"";

//      //run command and capture output
//      FILE* pipe=popen(command.c_str(),"r");
//      if(!pipe){
//         std::cout<<"Failed to fetch the data"<<std::endl;
//         return ;
//      }

//      //read output into string
//      char buffer[4096];
//      std::string jsonData="";
//      while(fgets(buffer,sizeof(buffer),pipe)!=NULL){
//         jsonData+=buffer;
//      }
//      pclose(pipe); 

//      //parse JSON
//      json data=json::parse(jsonData);

//      //loop through bids
//      for(auto &bid : data["bids"]){
//         std::string price=bid[0];
//         std::string qty=bid[1];
//         processLevel(price,qty,Side::BUY);
//      }

//      //loop through ask
//      for(auto &ask: data["asks"]){
//         std::string price=ask[0];
//         std::string qty=ask[1];
//         processLevel(price,qty,Side::SELL);
//      }

//      std::cout << "Total orders processed for " << symbol 
//           << ": " << processedCount << std::endl;

// }

//process level creates the order and sends it to exchange for processing
void MarketDataFeed::processLevel(std::string price, std::string qty, Side side){
    int64_t intprice=std::stod(price)*100;
    uint64_t intqty=std::stod(qty)*100000;
    Order order;
   order.side=side;
   order.symbol=this->symbol;
   order.price=intprice;
   order.quantity=intqty;
   order.orderType=OrderType::LIMIT;
   order.orderId=nextOrderId++;
   order.clientId=0;
   order.timeStamp=0;
   order.remainingQuantity=intqty;
   order.status=Status::NEW;
   //send order for processing to exchange
   exchange.processOrder(order);
   processedCount++;

}


