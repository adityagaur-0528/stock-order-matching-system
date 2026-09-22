#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include <map>
#include <queue>
#include <functional>
#include "Order.h"

class OrderBook
{
private:
    std::map<int, std::queue<Order>, std::greater<int>> buyOrders;
    std::map<int, std::queue<Order>> sellOrders;

public:
    bool orderExists(int id);

    bool addOrder(const Order& order);

    bool hasBuyOrders();

    bool hasSellOrders();

    Order& getBestBuyOrder();

    Order& getBestSellOrder();

    void removeBestBuy();

    void removeBestSell();

    void displayOrderBook();
};

#endif