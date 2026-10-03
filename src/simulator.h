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

    // Add money to balance
    void addBalance(double amount)
    {
        if (amount > 0)
        {
            balance += amount;
        }
    }

    // Deduct money from balance
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

    // Virtual Display Function
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
// TRADER CLASS
// Inherits from Participant
// ============================================================

class Trader : public Participant
{
private:

    // Stores already generated IDs
    static std::set<int> usedIds;

    // Generates a random unique ID
    static int generateUniqueId()
    {
        static std::mt19937 generator(
            std::random_device{}()
        );

        std::uniform_int_distribution<int> distribution(
            10000, 99999
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
    Trader()
        : Participant(generateUniqueId(), "", 0)
    {
    }

    // Parameterized Constructor
    Trader(std::string n, double b)
        : Participant(generateUniqueId(), n, b)
    {
    }

    // Overriding Display Function
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
};


// ============================================================
// ORDER CLASS
// ============================================================

class Order
{
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
        int i,
        int tId,
        char t,
        std::string s,
        int p,
        int q
    )
    {
        id = i;
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

    // Getter for Symbol
    std::string getSymbol() const
    {
        return symbol;
    }

    // Getter for Name
    std::string getName() const
    {
        return name;
    }

    // Getter for Current Price
    double getCurrentPrice() const
    {
        return currentPrice;
    }

    // Setter for Current Price
    void setCurrentPrice(double p)
    {
        if (p > 0)
        {
            currentPrice = p;
        }
    }

    // Display Stock Details
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
    std::cout
        << "\n========== AVAILABLE STOCKS =========="
        << std::endl;

    std::cout
        << "1. TCS"
        << " | Tata Consultancy Services"
        << " | Rs. 3500"
        << std::endl;

    std::cout
        << "2. INFY"
        << " | Infosys"
        << " | Rs. 1800"
        << std::endl;

    std::cout
        << "3. RELIANCE"
        << " | Reliance Industries"
        << " | Rs. 1400"
        << std::endl;

    std::cout
        << "4. HDFC"
        << " | HDFC Bank"
        << " | Rs. 1700"
        << std::endl;

    std::cout
        << "5. ITC"
        << " | ITC Limited"
        << " | Rs. 450"
        << std::endl;
}


// ============================================================
// RETURN STOCK ACCORDING TO USER CHOICE
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

#endif