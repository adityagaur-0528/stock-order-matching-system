
#ifndef TRADING_ENGINE_H
#define TRADING_ENGINE_H

#include <iostream>
#include <string>
#include "data_structures.h"

using namespace std;


class OrderBook
{
    string symbol;
    CustomBST buyOrders;
    CustomBST sellOrders;

public:
    OrderBook();
    explicit OrderBook(const string& symbol);
    const string& getSymbol() const;
    
    bool addOrder(const Order& order);
    bool cancelOrder(int orderId);
    bool getOrderById(int orderId, Order& order) const;
    bool hasBuyOrders() const;
    bool hasSellOrders() const;

    Order& getBestBuyOrder();
    Order& getBestSellOrder();

    bool removeBestBuy();
    bool removeBestSell();

    void emitStatus(ostream& out) const;
};
class TradingEngine
{
    static const int MAX_TRADERS = 200;
    static const int MAX_BOOKS = 100;

    Trader traders[MAX_TRADERS];
    int traderCount;

    OrderBook* books[MAX_BOOKS];
    int bookCount;
    
    Trader* findTrader(int traderId);
    const Trader* findTrader(int traderId) const;

    OrderBook* getOrderBook(const string& symbol);
    const OrderBook* getOrderBook(const string& symbol) const;

    bool executeMatching(
        OrderBook& book,
        char incomingType,
        ostream& out
    );

    void emitTrade(const Trade& trade, ostream& out) const;
    void emitAccount(const Trader& trader, ostream& out) const;
     public:
    TradingEngine();
    ~TradingEngine();
    TradingEngine(const TradingEngine&) = delete;
    TradingEngine& operator=(const TradingEngine&) = delete;

    
    void createTrader(const string& name, double balance, ostream& out);

    bool restoreTrader(int id, const string& name, double balance);

    bool seedHolding(int traderId, const string& symbol, int quantity);

    void allocateHolding(
        int traderId,
        const string& symbol,
        double price,
        int quantity,
        ostream& out
    );

    void addOrder(
        int traderId,
        char type,
        const string& symbol,
        double price,
        int quantity,
        ostream& out
    );

    void cancelOrder(int traderId, int orderId, ostream& out);

    void status(const string& symbol, ostream& out) const;

    void account(int traderId, ostream& out) const;
};

#endif
