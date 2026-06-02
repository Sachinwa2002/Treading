#include <map>
#include <deque>
#include<set>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <cstdint>
using namespace std;
enum class Side{
    BUY,
    SELL
};

enum class OrderType{
    LIMIT,
    MARKET
};

enum class Status{
    NEW,
    PARTIAL_FILL,
    FILLED,
    CANCELLED
};
struct Order{

    public: 
    Side side;
    string symbol;
    int64_t price;
    uint64_t quantity;
    OrderType orderType;
    uint64_t orderId;
    uint64_t clientId;
    uint64_t timeStamp;
    uint64_t remainingQuantity;
    Status status;
};
struct OrderLocation{
    int64_t price;
    Side side;
};
class OrderBook{

    private:
    map<int64_t,deque<Order>,greater<int64_t>> bids;
    map<int64_t, deque<Order>,less<int64_t>> asks;
    unordered_map<uint64_t,OrderLocation> orderIndex;
    unordered_set<uint64_t> cancelledOrders;

    public:
    void addOrder(Order order){

        if(order.side==Side::BUY){
          bids[order.price].push_back(order);
        }
        else{
          asks[order.price].push_back(order);
        }
        
        orderIndex[order.orderId]={order.price,order.side};
    }

    void cancelOrder(uint64_t orderId){
        cancelledOrders.insert(orderId);
        if(orderIndex.find(orderId)!=orderIndex.end()){
            int64_t price=orderIndex[orderId].price;
            if(orderIndex[orderId].side== Side::BUY){
                for(auto it=bids[price].begin();it!=bids[price].end();){
                      if(it->orderId==orderId){
                        it=bids[price].erase(it);
                        orderIndex.erase(orderId);
                        cancelledOrders.erase(orderId);
                        break;
                      }
                      else{
                        it++;
                      }
                }
                if(bids[price].empty()){
                    bids.erase(price);
                }
            }
            else{
                for(auto it=asks[price].begin();it!=asks[price].end();){
                    if(it->orderId==orderId){
                        it=asks[price].erase(it);
                        orderIndex.erase(orderId);
                        cancelledOrders.erase(orderId);
                        break;
                    }
                    else{
                        it++;
                    }
                }
                if(asks[price].empty()){
                    asks.erase(price);
                }
            }
        }

    }

    Order getBestBid(){
       if(bids.empty()){
        return Order{};
       }
       return bids.begin()->second.front();
    }

    Order getBestAsk(){
        if(asks.empty()){
            return Order{};
        }
        return asks.begin()->second.front();
    }

};
