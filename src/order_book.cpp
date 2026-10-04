#include <iostream>
#include "OrderBook.h"

using namespace std;

bool OrderBook::orderExists(int id)
{
    for (auto& entry : buyOrders)
    {
        queue<Order> temp = entry.second;

        while (!temp.empty())
        {
            if (temp.front().id == id)
                return true;

            temp.pop();
        }
    }

    for (auto& entry : sellOrders)
    {
        queue<Order> temp = entry.second;

        while (!temp.empty())
        {
            if (temp.front().id == id)
                return true;

            temp.pop();
        }
    }

    return false;
}

bool OrderBook::addOrder(const Order& order)
{
    if (orderExists(order.id))
    {
        cout << "Order ID already exists." << endl;
        return false;
    }

    if (order.type == 'B')
    {
        buyOrders[order.price].push(order);
    }
    else if (order.type == 'S')
    {
        sellOrders[order.price].push(order);
    }
    else
    {
        cout << "Invalid order type." << endl;
        return false;
    }

    return true;
}

bool OrderBook::hasBuyOrders()
{
    return !buyOrders.empty();
}

bool OrderBook::hasSellOrders()
{
    return !sellOrders.empty();
}

Order& OrderBook::getBestBuyOrder()
{
    return buyOrders.begin()->second.front();
}

Order& OrderBook::getBestSellOrder()
{
    return sellOrders.begin()->second.front();
}

void OrderBook::removeBestBuy()
{
    auto it = buyOrders.begin();

    it->second.pop();

    if (it->second.empty())
    {
        buyOrders.erase(it);
    }
}

void OrderBook::removeBestSell()
{
    auto it = sellOrders.begin();

    it->second.pop();

    if (it->second.empty())
    {
        sellOrders.erase(it);
    }
}

void OrderBook::displayOrderBook()
{
    cout << "\n========== BUY ORDERS ==========" << endl;

    for (auto& entry : buyOrders)
    {
        queue<Order> temp = entry.second;

        while (!temp.empty())
        {
            Order o = temp.front();

            cout << "ID: " << o.id
                 << " | Price: " << o.price
                 << " | Quantity: " << o.qty << endl;

            temp.pop();
        }
    }

    cout << "\n========== SELL ORDERS ==========" << endl;

    for (auto& entry : sellOrders)
    {
        queue<Order> temp = entry.second;

        while (!temp.empty())
        {
            Order o = temp.front();

            cout << "ID: " << o.id
                 << " | Price: " << o.price
                 << " | Quantity: " << o.qty << endl;

            temp.pop();
        }
    }
}