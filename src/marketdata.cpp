#include "json.hpp"
#include "marketdata.h"
#include <cstdio>
using json = nlohmann::json;

void MarketDataFeed:: fetchAndProcess(){
    //build URLS
    std::string url="https://api.binance.com/api/v3/depth?symbol="+symbol+"&limit=5";

    //build CURL command
     std::string command = "curl -s \"" + url + "\"";

     //run command and capture output
     FILE* pipe=popen(command.c_str(),"r");
     if(!pipe){
        std::cout<<"Failed to fetch the data"<<std::endl;
        return ;
     }

     //read output into string
     char buffer[4096];
     std::string jsonData="";
     while(fgets(buffer,sizeof(buffer),pipe)!=NULL){
        jsonData+=buffer;
     }
     pclose(pipe); 

     //parse JSON
     json data=json::parse(jsonData);

     //loop through bids
     for(auto &bid : data["bids"]){
        std::string price=bid[0];
        std::string qty=bid[1];
        processLevel(price,qty,Side::BUY);
     }

     //loop through ask
     for(auto &ask: data["asks"]){
        std::string price=ask[0];
        std::string qty=ask[1];
        processLevel(price,qty,Side::SELL);
     }

     std::cout << "Total orders processed for " << symbol 
          << ": " << processedCount << std::endl;

}
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
   exchange.processOrder(order);
   processedCount++;

}


