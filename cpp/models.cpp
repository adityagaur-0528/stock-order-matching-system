#include "models.h"
#include <algorithm>
#include <cmath>

using namespace std;
set<int> Trader::usedIds;
int Order::nextId = 1;

Participant::Participant()
    : id(0), name(""), balance(0.0), reservedCash(0.0) {}

Participant::Participant(int newId, const string& newName, double newBalance)
    : id(newId), name(newName), balance(newBalance), reservedCash(0.0) {}

int Participant::getId() const { return id; }

const string& Participant::getName() const { return name; }

double Participant::getBalance() const { return balance; }

double Participant::getReservedCash() const { return reservedCash; }

double Participant::getAvailableBalance() const
{
    return balance - reservedCash;
}

bool Participant::reserveCash(double amount)
{
    if (amount <= 0.0 || amount > getAvailableBalance() + 1e-9)
        return false;

    reservedCash += amount;
    return true;
}

void Participant::releaseCash(double amount)
{
    if (amount <= 0.0) return;

    reservedCash = max(0.0, reservedCash - amount);
}

bool Participant::deductBalance(double amount)
{
    if (amount <= 0.0 || amount > balance + 1e-9)
        return false;

    balance -= amount;
    return true;
}

void Participant::addBalance(double amount)
{
    if (amount > 0.0)
        balance += amount;
}

Holding::Holding() : symbol(""), quantity(0), reservedQuantity(0) {}

Holding::Holding(const string& newSymbol, int newQuantity)
    : symbol(newSymbol), quantity(newQuantity), reservedQuantity(0) {}
const string& Holding::getSymbol() const { return symbol; }

int Holding::getQuantity() const { return quantity; }

int Holding::getReservedQuantity() const { return reservedQuantity; }

int Holding::getAvailableQuantity() const
{
    return quantity - reservedQuantity;
}
bool Holding::addQuantity(int amount)
{
    if (amount <= 0) return false;

    quantity += amount;
    return true;
}
bool Holding::reserveQuantity(int amount)
{
    if (amount <= 0 || amount > getAvailableQuantity())
        return false;

    reservedQuantity += amount;
    return true;
}
void Holding::releaseReserved(int amount)
{
    if (amount <= 0) return;

    reservedQuantity = max(0, reservedQuantity - amount);
}
bool Holding::removeQuantity(int amount)
{
    if (amount <= 0 || amount > quantity - reservedQuantity)
        return false;

    quantity -= amount;
    return true;
}
int Trader::generateUniqueId()
{
    static mt19937 generator(random_device{}());
    uniform_int_distribution<int> distribution(10000, 99999);

    int newId;

    do
        newId = distribution(generator);
    while (usedIds.count(newId));

    usedIds.insert(newId);
    return newId;
}
Trader::Trader() : Participant(0, "", 0.0), holdingCount(0) {}

Trader::Trader(const string& newName, double newBalance)
    : Participant(generateUniqueId(), newName, newBalance), holdingCount(0) {}

Trader::Trader(int newId, const string& newName, double newBalance)
    : Participant(newId, newName, newBalance), holdingCount(0)
{
    usedIds.insert(newId);
}
int Trader::findHoldingIndex(const string& symbolName) const
{
    for (int i = 0; i < holdingCount; ++i)
        if (holdings[i].getSymbol() == symbolName)
            return i;

    return -1;
}
bool Trader::canBuy(double price, int quantity) const
{
    return price > 0.0 && quantity > 0 &&
           price * quantity <= getAvailableBalance() + 1e-9;
}
bool Trader::canSell(const string& symbolName, int quantity) const
{
    return quantity > 0 &&
           getAvailableHoldingQuantity(symbolName) >= quantity;
}
int Trader::getHoldingQuantity(const string& symbolName) const
{
    int index = findHoldingIndex(symbolName);
    return index == -1 ? 0 : holdings[index].getQuantity();
}
int Trader::getAvailableHoldingQuantity(const string& symbolName) const
{
    int index = findHoldingIndex(symbolName);
    return index == -1 ? 0 : holdings[index].getAvailableQuantity();
}
bool Trader::addHolding(const string& symbolName, int quantity)
{
    if (quantity <= 0) return false;

    int index = findHoldingIndex(symbolName);

    if (index != -1)
        return holdings[index].addQuantity(quantity);

    if (holdingCount >= 50) return false;

    holdings[holdingCount++] = Holding(symbolName, quantity);
    return true;
}
bool Trader::reserveHolding(const string& symbolName, int quantity)
{
    int index = findHoldingIndex(symbolName);
    return index != -1 && holdings[index].reserveQuantity(quantity);
}
void Trader::releaseReservedHolding(const string& symbolName, int quantity)
{
    int index = findHoldingIndex(symbolName);

    if (index != -1)
        holdings[index].releaseReserved(quantity);
}
bool Trader::removeHolding(const string& symbolName, int quantity)
{
    int index = findHoldingIndex(symbolName);

    if (index == -1) return false;

    if (!holdings[index].removeQuantity(quantity))
        return false;

    if (holdings[index].getQuantity() == 0)
    {
        for (int i = index; i < holdingCount - 1; ++i)
            holdings[i] = holdings[i + 1];

        --holdingCount;
    }

    return true;
}
int Trader::getHoldingCount() const
{
    return holdingCount;
}

const Holding& Trader::getHolding(int index) const
{
    return holdings[index];
}
Order::Order()
    : id(0), traderId(0), type(' '), symbol(""),
      price(0.0), qty(0), sequence(0) {}

Order::Order(int newTraderId, char newType, const string& newSymbol,double newPrice, int newQty)
     : id(nextId++), traderId(newTraderId), type(newType),symbol(newSymbol), price(newPrice), qty(newQty),sequence(id) {}
