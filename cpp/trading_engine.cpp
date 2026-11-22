#include "trading_engine.h"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <iostream>

using namespace std;

OrderBook::OrderBook() : symbol("") {}

OrderBook::OrderBook(const string& stockSymbol)
    : symbol(stockSymbol) {}

const string& OrderBook::getSymbol() const
{
    return symbol;
}
bool OrderBook::addOrder(const Order& order)
{
    if (order.symbol != symbol || order.qty <= 0 || order.price <= 0.0)
        return false;

    if (order.type == 'B')
    {
        return buyOrders.insert(order);
    }
    else if (order.type == 'S')
    {
        return sellOrders.insert(order);
    }
    else
    {
        return false;
    }
}
bool OrderBook::cancelOrder(int orderId)
{
    return buyOrders.cancelOrder(orderId) ||
           sellOrders.cancelOrder(orderId);
}
bool OrderBook::getOrderById(int orderId, Order& order) const
{
    return buyOrders.getOrderById(orderId, order) ||
           sellOrders.getOrderById(orderId, order);
}
bool OrderBook::hasBuyOrders() const
{
    return !buyOrders.isEmpty();
}

bool OrderBook::hasSellOrders() const
{
    return !sellOrders.isEmpty();
}
Order& OrderBook::getBestBuyOrder()
{
    return buyOrders.getMaxOrder();
}

Order& OrderBook::getBestSellOrder()
{
    return sellOrders.getMinOrder();
}
bool OrderBook::removeBestBuy()
{
    return buyOrders.removeMaxOrder();
}

bool OrderBook::removeBestSell()
{
    return sellOrders.removeMinOrder();
}
void OrderBook::emitStatus(ostream& out) const
{
    out << "SIDE|BUY\n";
    buyOrders.emitDescending(out);

    out << "SIDE|SELL\n";
    sellOrders.emitAscending(out);
}

void TradingEngine::emitTrade(const Trade& t, ostream& out) const
{
    out << "TRADE|" << t.buyOrderId << "|"
        << t.sellOrderId << "|"
        << t.buyTraderId << "|"
        << t.sellTraderId << "|"
        << t.symbol << "|"
        << fixed << setprecision(2)
        << t.price << "|" << t.quantity << "\n";
}

TradingEngine::TradingEngine()
    : traderCount(0), bookCount(0)
{
    for (int i = 0; i < MAX_BOOKS; i++)
    {
        books[i] = nullptr;
    }
}

TradingEngine::~TradingEngine()
{
    for (int i = 0; i < bookCount; i++)
    {
        delete books[i];
        books[i] = nullptr;
    }
}

Trader* TradingEngine::findTrader(int traderId)
{
    for (int i = 0; i < traderCount; i++)
    {
        if (traders[i].getId() == traderId)
        {
            return &traders[i];
        }
    }

    return nullptr;
}

const Trader* TradingEngine::findTrader(int traderId) const
{
    for (int i = 0; i < traderCount; i++)
    {
        if (traders[i].getId() == traderId)
        {
            return &traders[i];
        }
    }

    return nullptr;
}

OrderBook* TradingEngine::getOrderBook(const string& symbol)
{
    for (int i = 0; i < bookCount; i++)
    {
        if (books[i]->getSymbol() == symbol)
        {
            return books[i];
        }
    }

    if (bookCount >= MAX_BOOKS)
    {
        return nullptr;
    }

    books[bookCount] = new OrderBook(symbol);
    bookCount++;

    return books[bookCount - 1];
}

const OrderBook* TradingEngine::getOrderBook(const string& symbol) const
{
    for (int i = 0; i < bookCount; i++)
    {
        if (books[i]->getSymbol() == symbol)
        {
            return books[i];
        }
    }

    return nullptr;
}

void TradingEngine::createTrader(const string& name, double balance, ostream& out)
{
    if (traderCount >= MAX_TRADERS || name.empty() ||
        !isfinite(balance) || balance <= 0.0)
    {
        out << "ERROR|Invalid trader details or trader limit reached\nEND\n";
        return;
    }

    Trader created(name, balance);
    traders[traderCount++] = created;

    out << "TRADER_CREATED|" << created.getId() << "|"
        << created.getName() << "|"
        << fixed << setprecision(2) << created.getBalance()
        << "\nEND\n";
}

bool TradingEngine::restoreTrader(int id, const string& name, double balance)
{
    if (id <= 0 || name.empty() || !isfinite(balance) ||
        balance < 0.0 || traderCount >= MAX_TRADERS)
    {
        return false;
    }

    if (findTrader(id))
    {
        return true;
    }

    traders[traderCount++] = Trader(id, name, balance);
    return true;
}

bool TradingEngine::seedHolding(int traderId, const string& symbol, int quantity)
{
    Trader* trader = findTrader(traderId);

    if (!trader || symbol.empty() || quantity <= 0)
    {
        return false;
    }

    return trader->addHolding(symbol, quantity);
}

void TradingEngine::allocateHolding(int traderId, const string& symbol,
                                    double price, int quantity, ostream& out)
{
    Trader* trader = findTrader(traderId);

    if (!trader || symbol.empty() || !isfinite(price) ||
        price <= 0.0 || quantity <= 0)
    {
        out << "ERROR|Invalid allocation details\nEND\n";
        return;
    }

    const double cost = price * quantity;

    if (!trader->canBuy(price, quantity))
    {
        out << "ERROR|Insufficient available cash\nEND\n";
        return;
    }

    if (!trader->addHolding(symbol, quantity))
    {
        out << "ERROR|Unable to add holding (portfolio capacity reached)\nEND\n";
        return;
    }

    if (!trader->deductBalance(cost))
    {
        trader->removeHolding(symbol, quantity);
        out << "ERROR|Unable to settle allocation cost\nEND\n";
        return;
    }

    out << "ALLOCATION_OK|" << traderId << "|" << symbol << "|"
        << fixed << setprecision(2) << price << "|"
        << quantity << "|" << cost << "\n";

    emitAccount(*trader, out);
    out << "END\n";
}

bool TradingEngine::executeMatching(OrderBook& book, int incomingOrderId, ostream& out)
{
    bool traded = false;

    while (book.hasBuyOrders() && book.hasSellOrders())
    {
        Order& buyRef = book.getBestBuyOrder();
        Order& sellRef = book.getBestSellOrder();

        if (buyRef.price + 1e-9 < sellRef.price)
            break;

        // Prevent a trader from buying their own shares
        if (buyRef.traderId == sellRef.traderId)
        {
            Order incoming;

            if (!book.getOrderById(incomingOrderId, incoming))
                break;

            // The incoming order must be one of the conflicting orders
            if (incoming.id != buyRef.id && incoming.id != sellRef.id)
                break;

            if (!book.cancelOrder(incomingOrderId))
                break;

            Trader* owner = findTrader(incoming.traderId);

            if (owner)
            {
                if (incoming.type == 'B')
                    owner->releaseCash(incoming.price * incoming.qty);
                else if (incoming.type == 'S')
                    owner->releaseReservedHolding(
                        incoming.symbol, incoming.qty
                    );
            }

            out << "SELF_TRADE_PREVENTED|"
                << incoming.id << "|"
                << incoming.traderId << "\n";

            out << "CANCELLED|"
                << incoming.id << "|"
                << incoming.traderId << "|"
                << incoming.type << "|"
                << incoming.symbol << "|"
                << fixed << setprecision(2)
                << incoming.price << "|"
                << incoming.qty << "\n";

            if (owner)
                emitAccount(*owner, out);

            break;
        }

        const double tradePrice =
            (buyRef.sequence < sellRef.sequence)
                ? buyRef.price
                : sellRef.price;

        const int quantity = min(buyRef.qty, sellRef.qty);

        const Order buy = buyRef;
        const Order sell = sellRef;

        Trader* buyer = findTrader(buy.traderId);
        Trader* seller = findTrader(sell.traderId);

        if (!buyer || !seller || quantity <= 0)
            break;

        buyer->releaseCash(buy.price * quantity);

        if (!buyer->deductBalance(tradePrice * quantity))
        {
            buyer->reserveCash(buy.price * quantity);
            out << "ERROR|Settlement failed: buyer cash unavailable\n";
            break;
        }

        seller->releaseReservedHolding(sell.symbol, quantity);

        if (!seller->removeHolding(sell.symbol, quantity))
        {
            buyer->addBalance(tradePrice * quantity);
            buyer->addHolding(buy.symbol, quantity);

            out << "ERROR|Settlement failed: seller shares unavailable\n";
            break;
        }

        buyer->addHolding(buy.symbol, quantity);
        seller->addBalance(tradePrice * quantity);

        buyRef.qty -= quantity;
        sellRef.qty -= quantity;

        emitTrade(
            {buy.id, sell.id, buy.traderId, sell.traderId,
             buy.symbol, tradePrice, quantity},
            out
        );

        traded = true;

        if (buyRef.qty == 0)
            book.removeBestBuy();

        if (sellRef.qty == 0)
            book.removeBestSell();
    }

    return traded;
}

void TradingEngine::addOrder(int traderId, char type,const string& symbol, double price,int quantity, ostream& out)
{
    Trader* trader = findTrader(traderId);

    type = static_cast<char>(
        toupper(static_cast<unsigned char>(type))
    );

    if (!trader || (type != 'B' && type != 'S') ||symbol.empty() || !isfinite(price) || price <= 0.0 || quantity <= 0)
    {
        out << "ERROR|Invalid trader, side, symbol, price, or quantity\nEND\n";
        return;
    }

    const double reserve = price * quantity;

    if (!isfinite(reserve))
    {
        out << "ERROR|Order value is too large\nEND\n";
        return;
    }

    if (type == 'B')
    {
        if (!trader->reserveCash(reserve))
        {
            out << "ERROR|Insufficient available cash for this BUY order\nEND\n";
            return;
        }
    }
    else
    {
        if (!trader->reserveHolding(symbol, quantity))
        {
            out << "ERROR|Insufficient available shares to SELL\nEND\n";
            return;
        }
    }

    Order order(traderId, type, symbol, price, quantity);
    OrderBook* book = getOrderBook(symbol);

    if (!book || !book->addOrder(order))
    {
        if (type == 'B')
        {
            trader->releaseCash(reserve);
        }
        else
        {
            trader->releaseReservedHolding(symbol, quantity);
        }

        out << "ERROR|Order book is full or order could not be added\nEND\n";
        return;
    }

    out << "ORDER_ADDED|" << order.id << "|"
        << order.traderId << "|" << order.type << "|"
        << order.symbol << "|" << fixed << setprecision(2)
        << order.price << "|" << order.qty << "\n";

    executeMatching(*book, order.id, out);

    out << "END\n";
}

void TradingEngine::cancelOrder(int traderId, int orderId, ostream& out)
{
    Trader* trader = findTrader(traderId);
    if (!trader)
    {
        out << "ERROR|Trader not found\nEND\n";
        return;
    }
    if (orderId <= 0)
    {
        out << "ERROR|Invalid order ID\nEND\n";
        return;
    }

    Order found;
    OrderBook* owningBook = nullptr;
    for (int i = 0; i < bookCount; ++i)
    {
        if (books[i]->getOrderById(orderId, found))
        {
            owningBook = books[i];
            break;
        }
    }

    if (!owningBook)
    {
        out << "ERROR|Order is not open or does not exist\nEND\n";
        return;
    }
    if (found.traderId != traderId)
    {
        out << "ERROR|You can cancel only your own orders\nEND\n";
        return;
    }

    // Capture details before removal so only the still-open quantity is released.
    if (!owningBook->cancelOrder(orderId))
    {
        out << "ERROR|Order could not be cancelled\nEND\n";
        return;
    }

    if (found.type == 'B')
        trader->releaseCash(found.price * found.qty);
    else if (found.type == 'S')
        trader->releaseReservedHolding(found.symbol, found.qty);

    out << "CANCELLED|" << found.id << "|" << found.traderId << "|"
        << found.type << "|" << found.symbol << "|"
        << fixed << setprecision(2) << found.price << "|"
        << found.qty << "\n";
    emitAccount(*trader, out);
    out << "END\n";
}

void TradingEngine::status(const string& symbol, ostream& out) const
{
    const OrderBook* book = getOrderBook(symbol);

    out << "STATUS|" << symbol << "\n";

    if (book)
    {
        book->emitStatus(out);
    }
    else
    {
        out << "SIDE|BUY\nSIDE|SELL\n";
    }

    out << "END\n";
}

void TradingEngine::emitAccount(const Trader& trader, ostream& out) const
{
    out << "ACCOUNT|" << trader.getId() << "|"
        << fixed << setprecision(2)
        << trader.getBalance() << "|"
        << trader.getReservedCash() << "|"
        << trader.getAvailableBalance() << "|"
        << trader.getHoldingCount();

    for (int i = 0; i < trader.getHoldingCount(); i++)
    {
        const Holding& h = trader.getHolding(i);

        out << "|" << h.getSymbol()
            << "|" << h.getQuantity()
            << "|" << h.getReservedQuantity()
            << "|" << h.getAvailableQuantity();
    }

    out << "\n";
}

void TradingEngine::account(int traderId, ostream& out) const
{
    const Trader* trader = findTrader(traderId);

    if (!trader)
    {
        out << "ERROR|Trader not found\nEND\n";
    }
    else
    {
        emitAccount(*trader, out);
        out << "END\n";
    }
}


