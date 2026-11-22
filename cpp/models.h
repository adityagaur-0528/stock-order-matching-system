#ifndef MODELS_H
#define MODELS_H
#include<iostream>
#include<random>
#include<string>
#include<set>
#include<vector>
using namespace std;

class Participant {
    protected:
    
    int id;
     string name;
     double balance;
     double reservedCash;
    public:
      Participant();
      Participant(int, const string&, double);
      virtual ~Participant() = default;

    int getId() const;
    const string& getName() const;
    double getBalance() const;
    double getReservedCash() const;
    double getAvailableBalance() const;

    bool reserveCash(double);
    void releaseCash(double);
    bool deductBalance(double);
    void addBalance(double);
};

class Holding 
{
    string symbol;
    int quantity;
    int reservedQuantity;
    public:
     Holding();
     Holding(const string&, int);
    const string& getSymbol() const;
    int getQuantity() const;
    int getReservedQuantity() const;
    int getAvailableQuantity() const;

    bool addQuantity(int);
    bool reserveQuantity(int);
    void releaseReserved(int);
    bool removeQuantity(int);
};

class Trader : public Participant
{
    static set<int> usedIds;
    Holding holdings[50];
    int holdingCount;

    static int generateUniqueId();
    int findHoldingIndex(const string&) const;

public:
    Trader();
    Trader(const string&, double);
    Trader(int, const string&, double);

    bool canBuy(double, int) const;
    bool canSell(const string&, int) const;

    int getHoldingQuantity(const string&) const;
    int getAvailableHoldingQuantity(const string&) const;

    bool addHolding(const string&, int);
    bool reserveHolding(const string&, int);
    void releaseReservedHolding(const string&, int);
    bool removeHolding(const string&, int);

    int getHoldingCount() const;
    const Holding& getHolding(int) const;
};

class Order {
    static int nextId;

public:
    int id;
    int traderId;
    char type;
    string symbol;
    double price;
    int qty;
    long long sequence;

    Order();
    Order(int, char, const string&, double, int);
};

struct Trade {
    int buyOrderId;
    int sellOrderId;
    int buyTraderId;
    int sellTraderId;
    string symbol;
    double price;
    int quantity;
};

#endif