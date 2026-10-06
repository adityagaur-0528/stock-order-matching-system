#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <string>
#include <set>
#include <random>


// ============================================================
// PARTICIPANT CLASS
// ============================================================

class Participant
{
protected:
    int id;
    std::string name;
    double balance;

public:

    // Default Constructor
    Participant()
    {
        id = 0;
        name = "";
        balance = 0;
    }

    // Parameterized Constructor
    Participant(int i, std::string n, double b)
    {
        id = i;
        name = n;
        balance = b;
    }

    // Virtual Destructor
    virtual ~Participant()
    {
    }

    // Getter for ID
    int getId() const
    {
        return id;
    }

    // Getter for Name
    std::string getName() const
    {
        return name;
    }

    // Getter for Balance
    double getBalance() const
    {
        return balance;
    }

    // Add money
    void addBalance(double amount)
    {
        if (amount > 0)
        {
            balance += amount;
        }
    }

    // Deduct money
    bool deductBalance(double amount)
    {
        if (amount <= 0)
        {
            return false;
        }

        if (amount > balance)
        {
            return false;
        }

        balance -= amount;
        return true;
    }

    // Virtual Display
    virtual void display() const
    {
        std::cout << "\n---------- PARTICIPANT ----------"
                  << std::endl;

        std::cout << "Participant ID: "
                  << id << std::endl;

        std::cout << "Name: "
                  << name << std::endl;

        std::cout << "Balance: Rs. "
                  << balance << std::endl;
    }
};


// ============================================================
// HOLDING CLASS
// ============================================================

class Holding
{
private:
    std::string symbol;
    int quantity;

public:

    // Default Constructor
    Holding()
    {
        symbol = "";
        quantity = 0;
    }

    // Parameterized Constructor
    Holding(std::string s, int q)
    {
        symbol = s;
        quantity = q;
    }

    // Getter for Symbol
    std::string getSymbol() const
    {
        return symbol;
    }

    // Getter for Quantity
    int getQuantity() const
    {
        return quantity;
    }

    // Add shares
    void addQuantity(int q)
    {
        if (q > 0)
        {
            quantity += q;
        }
    }

    // Remove shares
    bool removeQuantity(int q)
    {
        if (q <= 0 || q > quantity)
        {
            return false;
        }

        quantity -= q;
        return true;
    }
};


// ============================================================
// TRADER CLASS
// Inherits from Participant
// ============================================================

class Trader : public Participant
{
private:

    // Already generated Trader IDs
    static std::set<int> usedIds;

    // Trader holdings
    Holding holdings[20];

    // Number of different stocks held
    int holdingCount;

    // Generate random unique ID
    static int generateUniqueId()
    {
        static std::mt19937 generator(
            std::random_device{}()
        );

        std::uniform_int_distribution<int> distribution(
            10000,
            99999
        );

        int newId;

        do
        {
            newId = distribution(generator);
        }
        while (usedIds.find(newId) != usedIds.end());

        usedIds.insert(newId);

        return newId;
    }

public:

    // Default Constructor
    // Does NOT generate an ID.
    // This is useful for trader arrays.
    Trader()
        : Participant(0, "", 0)
    {
        holdingCount = 0;
    }

    // Parameterized Constructor
    Trader(std::string n, double b)
        : Participant(generateUniqueId(), n, b)
    {
        holdingCount = 0;
    }

    // Get holding quantity of a stock
    int getHoldingQuantity(std::string symbol) const
    {
        for (int i = 0; i < holdingCount; i++)
        {
            if (holdings[i].getSymbol() == symbol)
            {
                return holdings[i].getQuantity();
            }
        }

        return 0;
    }

    // Check whether trader can buy
    bool canBuy(int price, int quantity) const
    {
        if (price <= 0 || quantity <= 0)
        {
            return false;
        }

        double totalCost =
            static_cast<double>(price) * quantity;

        return totalCost <= balance;
    }

    // Check whether trader can sell
    bool canSell(
        std::string symbol,
        int quantity
    ) const
    {
        if (quantity <= 0)
        {
            return false;
        }

        return getHoldingQuantity(symbol) >= quantity;
    }

    // Add stock to holdings
    bool addHolding(
        std::string symbol,
        int quantity
    )
    {
        if (quantity <= 0)
        {
            return false;
        }

        // Stock already exists
        for (int i = 0; i < holdingCount; i++)
        {
            if (holdings[i].getSymbol() == symbol)
            {
                holdings[i].addQuantity(quantity);
                return true;
            }
        }

        // Maximum number of different stocks
        if (holdingCount >= 20)
        {
            return false;
        }

        holdings[holdingCount] =
            Holding(symbol, quantity);

        holdingCount++;

        return true;
    }

    // Remove stock from holdings
    bool removeHolding(
        std::string symbol,
        int quantity
    )
    {
        if (!canSell(symbol, quantity))
        {
            return false;
        }

        for (int i = 0; i < holdingCount; i++)
        {
            if (holdings[i].getSymbol() == symbol)
            {
                holdings[i].removeQuantity(quantity);

                // If quantity becomes zero,
                // shift remaining holdings left.
                if (holdings[i].getQuantity() == 0)
                {
                    for (int j = i; j < holdingCount - 1; j++)
                    {
                        holdings[j] = holdings[j + 1];
                    }

                    holdingCount--;
                }

                return true;
            }
        }

        return false;
    }

    // Display Trader
    void display() const override
    {
        std::cout << "\n---------- TRADER ----------"
                  << std::endl;

        std::cout << "Trader ID: "
                  << id << std::endl;

        std::cout << "Name: "
                  << name << std::endl;

        std::cout << "Balance: Rs. "
                  << balance << std::endl;
    }

    // Display Holdings
    void displayHoldings() const
    {
        std::cout << "\n---------- HOLDINGS ----------"
                  << std::endl;

        std::cout << "Trader: "
                  << name << std::endl;

        if (holdingCount == 0)
        {
            std::cout << "No holdings."
                      << std::endl;
            return;
        }

        for (int i = 0; i < holdingCount; i++)
        {
            std::cout
                << holdings[i].getSymbol()
                << " | Quantity: "
                << holdings[i].getQuantity()
                << std::endl;
        }
    }
};


// ============================================================
// ORDER CLASS
// ============================================================

class Order
{
private:

    // Automatically generated Order ID
    static inline int nextId = 1;

public:

    int id;
    int traderId;
    char type;
    std::string symbol;
    int price;
    int qty;

    // Default Constructor
    Order()
    {
        id = 0;
        traderId = 0;
        type = ' ';
        symbol = "";
        price = 0;
        qty = 0;
    }

    // Parameterized Constructor
    Order(
        int tId,
        char t,
        std::string s,
        int p,
        int q
    )
    {
        id = nextId++;
        traderId = tId;
        type = t;
        symbol = s;
        price = p;
        qty = q;
    }
};


// ============================================================
// STOCK CLASS
// ============================================================

class Stock
{
private:
    std::string symbol;
    std::string name;
    double currentPrice;

public:

    // Default Constructor
    Stock()
    {
        symbol = "";
        name = "";
        currentPrice = 0;
    }

    // Parameterized Constructor
    Stock(
        std::string s,
        std::string n,
        double p
    )
    {
        symbol = s;
        name = n;
        currentPrice = p;
    }

    // Get Symbol
    std::string getSymbol() const
    {
        return symbol;
    }

    // Get Name
    std::string getName() const
    {
        return name;
    }

    // Get Current Price
    double getCurrentPrice() const
    {
        return currentPrice;
    }

    // Set Current Price
    void setCurrentPrice(double p)
    {
        if (p > 0)
        {
            currentPrice = p;
        }
    }

    // Display Stock
    void display() const
    {
        std::cout << "\n---------- STOCK ----------"
                  << std::endl;

        std::cout << "Symbol: "
                  << symbol << std::endl;

        std::cout << "Name: "
                  << name << std::endl;

        std::cout << "Current Price: Rs. "
                  << currentPrice << std::endl;
    }
};


// ============================================================
// DISPLAY AVAILABLE STOCKS
// ============================================================

inline void displayAvailableStocks()
{
    std::cout << "\n========== AVAILABLE STOCKS =========="
              << std::endl;

    std::cout << "1. TCS"
              << " | Tata Consultancy Services"
              << " | Rs. 3500"
              << std::endl;

    std::cout << "2. INFY"
              << " | Infosys"
              << " | Rs. 1800"
              << std::endl;

    std::cout << "3. RELIANCE"
              << " | Reliance Industries"
              << " | Rs. 1400"
              << std::endl;

    std::cout << "4. HDFC"
              << " | HDFC Bank"
              << " | Rs. 1700"
              << std::endl;

    std::cout << "5. ITC"
              << " | ITC Limited"
              << " | Rs. 450"
              << std::endl;
}


// ============================================================
// GET STOCK
// ============================================================

inline Stock getStock(int choice)
{
    switch (choice)
    {
        case 1:
            return Stock(
                "TCS",
                "Tata Consultancy Services",
                3500
            );

        case 2:
            return Stock(
                "INFY",
                "Infosys",
                1800
            );

        case 3:
            return Stock(
                "RELIANCE",
                "Reliance Industries",
                1400
            );

        case 4:
            return Stock(
                "HDFC",
                "HDFC Bank",
                1700
            );

        case 5:
            return Stock(
                "ITC",
                "ITC Limited",
                450
            );

        default:
            return Stock();
    }
}


// ============================================================
// FIND TRADER BY ID
// ============================================================

inline Trader* findTraderById(
    int traderId,
    Trader traders[],
    int traderCount
)
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


// ============================================================
// REMOVE ORDER FROM ARRAY
// ============================================================

inline void removeOrder(
    Order orders[],
    int& orderCount,
    int index
)
{
    for (int i = index; i < orderCount - 1; i++)
    {
        orders[i] = orders[i + 1];
    }

    orderCount--;
}


// ============================================================
// MATCH ORDERS
// ============================================================

inline void matchOrders(
    Order orders[],
    int& orderCount,
    Trader traders[],
    int traderCount
)
{
    bool tradeExecuted = false;

    while (true)
    {
        int bestBuy = -1;
        int bestSell = -1;

        // ----------------------------------------------------
        // Find best BUY and SELL orders
        // ----------------------------------------------------

        for (int i = 0; i < orderCount; i++)
        {
            if (orders[i].qty <= 0)
            {
                continue;
            }

            if (orders[i].type != 'B')
            {
                continue;
            }

            for (int j = 0; j < orderCount; j++)
            {
                if (orders[j].qty <= 0)
                {
                    continue;
                }

                if (orders[j].type != 'S')
                {
                    continue;
                }

                // Same stock
                if (orders[i].symbol != orders[j].symbol)
                {
                    continue;
                }

                // Trader cannot trade with himself
                if (orders[i].traderId == orders[j].traderId)
                {
                    continue;
                }

                // Price compatibility
                if (orders[i].price < orders[j].price)
                {
                    continue;
                }

                // First compatible order pair
                if (bestBuy == -1)
                {
                    bestBuy = i;
                    bestSell = j;
                    continue;
                }

                // Higher BUY price gets priority
                if (orders[i].price >
                    orders[bestBuy].price)
                {
                    bestBuy = i;
                    bestSell = j;
                    continue;
                }

                // Same BUY price -> earlier BUY order
                if (orders[i].price ==
                        orders[bestBuy].price &&
                    orders[i].id <
                        orders[bestBuy].id)
                {
                    bestBuy = i;
                    bestSell = j;
                    continue;
                }

                // Same BUY -> lower SELL price
                if (orders[i].price ==
                        orders[bestBuy].price &&
                    orders[i].id ==
                        orders[bestBuy].id &&
                    orders[j].price <
                        orders[bestSell].price)
                {
                    bestBuy = i;
                    bestSell = j;
                    continue;
                }

                // Same prices -> earlier SELL
                if (orders[i].price ==
                        orders[bestBuy].price &&
                    orders[i].id ==
                        orders[bestBuy].id &&
                    orders[j].price ==
                        orders[bestSell].price &&
                    orders[j].id <
                        orders[bestSell].id)
                {
                    bestBuy = i;
                    bestSell = j;
                }
            }
        }


        // ----------------------------------------------------
        // No compatible trade
        // ----------------------------------------------------

        if (bestBuy == -1 || bestSell == -1)
        {
            if (!tradeExecuted)
            {
                std::cout
                    << "\nNo compatible orders available."
                    << std::endl;
            }

            break;
        }


        // ----------------------------------------------------
        // Find Buyer and Seller
        // ----------------------------------------------------

        Trader* buyer = findTraderById(
            orders[bestBuy].traderId,
            traders,
            traderCount
        );

        Trader* seller = findTraderById(
            orders[bestSell].traderId,
            traders,
            traderCount
        );


        if (buyer == nullptr || seller == nullptr)
        {
            std::cout
                << "\nTrader not found."
                << std::endl;

            break;
        }


        // ----------------------------------------------------
        // Determine Quantity
        // ----------------------------------------------------

        int tradeQuantity;

        if (orders[bestBuy].qty <
            orders[bestSell].qty)
        {
            tradeQuantity =
                orders[bestBuy].qty;
        }
        else
        {
            tradeQuantity =
                orders[bestSell].qty;
        }


        // Current project execution price rule
        // Both prices will normally be equal
        int tradePrice =
            orders[bestSell].price;


        double tradeValue =
            static_cast<double>(tradePrice)
            * tradeQuantity;


        // ----------------------------------------------------
        // Check BUYER balance
        // ----------------------------------------------------

        if (!buyer->canBuy(
                tradePrice,
                tradeQuantity))
        {
            std::cout
                << "\nTrade rejected for Order ID "
                << orders[bestBuy].id
                << "." << std::endl;

            std::cout
                << "Buyer has insufficient balance."
                << std::endl;

            orders[bestBuy].qty = 0;

            removeOrder(
                orders,
                orderCount,
                bestBuy
            );

            continue;
        }


        // ----------------------------------------------------
        // Check SELLER holdings
        // ----------------------------------------------------

        if (!seller->canSell(
                orders[bestSell].symbol,
                tradeQuantity))
        {
            std::cout
                << "\nTrade rejected for Order ID "
                << orders[bestSell].id
                << "." << std::endl;

            std::cout
                << "Seller has insufficient holdings."
                << std::endl;

            orders[bestSell].qty = 0;

            removeOrder(
                orders,
                orderCount,
                bestSell
            );

            continue;
        }


        // ----------------------------------------------------
        // Execute Trade
        // ----------------------------------------------------

        buyer->deductBalance(tradeValue);

        seller->addBalance(tradeValue);

        buyer->addHolding(
            orders[bestBuy].symbol,
            tradeQuantity
        );

        seller->removeHolding(
            orders[bestSell].symbol,
            tradeQuantity
        );


        // Update Remaining Quantity

        orders[bestBuy].qty -=
            tradeQuantity;

        orders[bestSell].qty -=
            tradeQuantity;


        tradeExecuted = true;


        // ----------------------------------------------------
        // Display Trade
        // ----------------------------------------------------

        std::cout
            << "\n========== TRADE EXECUTED =========="
            << std::endl;

        std::cout
            << "Buyer: "
            << buyer->getName()
            << " (ID: "
            << buyer->getId()
            << ")"
            << std::endl;

        std::cout
            << "Seller: "
            << seller->getName()
            << " (ID: "
            << seller->getId()
            << ")"
            << std::endl;

        std::cout
            << "Stock: "
            << orders[bestBuy].symbol
            << std::endl;

        std::cout
            << "Trade Price: Rs. "
            << tradePrice
            << std::endl;

        std::cout
            << "Trade Quantity: "
            << tradeQuantity
            << std::endl;

        std::cout
            << "Trade Value: Rs. "
            << tradeValue
            << std::endl;


        // ----------------------------------------------------
        // Remove Completed Orders
        // ----------------------------------------------------

        bool buyCompleted =
            orders[bestBuy].qty == 0;

        bool sellCompleted =
            orders[bestSell].qty == 0;


        if (buyCompleted &&
            sellCompleted)
        {
            // Remove higher index first
            if (bestBuy > bestSell)
            {
                removeOrder(
                    orders,
                    orderCount,
                    bestBuy
                );

                removeOrder(
                    orders,
                    orderCount,
                    bestSell
                );
            }
            else
            {
                removeOrder(
                    orders,
                    orderCount,
                    bestSell
                );

                removeOrder(
                    orders,
                    orderCount,
                    bestBuy
                );
            }
        }
        else if (buyCompleted)
        {
            removeOrder(
                orders,
                orderCount,
                bestBuy
            );
        }
        else if (sellCompleted)
        {
            removeOrder(
                orders,
                orderCount,
                bestSell
            );
        }
    }
}

#endif