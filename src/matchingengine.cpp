#include <iostream>
#include <algorithm>
#include "MatchingEngine.h"
#include "OrderManagement.h"

using namespace std;

void matchOrders(OrderBook& book)
{
    while (book.hasBuyOrders() && book.hasSellOrders())
    {
        Order& buyOrder = book.getBestBuyOrder();
        Order& sellOrder = book.getBestSellOrder();

        // Check matching condition
        if (buyOrder.price < sellOrder.price)
        {
            cout << "\nNo compatible orders available." << endl;
            break;
        }

        // Save IDs before possible removal
        int buyId = buyOrder.id;
        int sellId = sellOrder.id;

        // Current Phase-I execution price rule
        int tradePrice = sellOrder.price;

        // Calculate executable quantity
        int tradeQuantity = min(buyOrder.qty, sellOrder.qty);

        cout << "\n========== TRADE EXECUTED ==========" << endl;
        cout << "Buy Order ID: " << buyId << endl;
        cout << "Sell Order ID: " << sellId << endl;
        cout << "Trade Price: Rs. " << tradePrice << endl;
        cout << "Trade Quantity: " << tradeQuantity << endl;

        // Order Management updates quantities
        updateOrder(buyOrder, tradeQuantity);
        updateOrder(sellOrder, tradeQuantity);

        bool buyCompleted = isCompleted(buyOrder);
        bool sellCompleted = isCompleted(sellOrder);

        // Remove completed orders
        if (buyCompleted)
        {
            cout << "Buy Order " << buyId
                 << " completed." << endl;

            book.removeBestBuy();
        }

        if (sellCompleted)
        {
            cout << "Sell Order " << sellId
                 << " completed." << endl;

            book.removeBestSell();
        }
    }
}