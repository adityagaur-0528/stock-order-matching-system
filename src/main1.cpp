#include <iostream>
#include "simulator.h"

using namespace std;

int main()
{
    Trader t1("Aditya", 100000);
    Trader t2("Utkarsh", 100000);
    Trader t3("Shivam", 100000);

    cout << "========================================" << endl;
    cout << "     STOCK ORDER MATCHING SIMULATOR" << endl;
    cout << "========================================" << endl;

    cout << "\n========== AVAILABLE TRADERS ==========" << endl;

    cout << "1. " << t1.getName()
         << " | Trader ID: " << t1.getId()
         << " | Balance: Rs. " << t1.getBalance()
         << endl;

    cout << "2. " << t2.getName()
         << " | Trader ID: " << t2.getId()
         << " | Balance: Rs. " << t2.getBalance()
         << endl;

    cout << "3. " << t3.getName()
         << " | Trader ID: " << t3.getId()
         << " | Balance: Rs. " << t3.getBalance()
         << endl;

    int traderChoice;

    cout << "\nSelect Trader: ";
    cin >> traderChoice;

    Trader* selectedTrader = nullptr;

    switch (traderChoice)
    {
        case 1:
            selectedTrader = &t1;
            break;

        case 2:
            selectedTrader = &t2;
            break;

        case 3:
            selectedTrader = &t3;
            break;

        default:
            cout << "Invalid trader choice." << endl;
            return 0;
    }

    displayAvailableStocks();

    int stockChoice;

    cout << "\nSelect Stock: ";
    cin >> stockChoice;

    Stock selectedStock = getStock(stockChoice);

    if (selectedStock.getSymbol() == "")
    {
        cout << "Invalid stock choice." << endl;
        return 0;
    }

    char type;

    cout << "\nEnter Order Type (B/S): ";
    cin >> type;

    if (type != 'B' && type != 'S')
    {
        cout << "Invalid order type." << endl;
        return 0;
    }

    int price;
    int quantity;

    cout << "Enter Price: ";
    cin >> price;

    cout << "Enter Quantity: ";
    cin >> quantity;

    if (price <= 0 || quantity <= 0)
    {
        cout << "Invalid price or quantity." << endl;
        return 0;
    }

    Order order(
        1,
        selectedTrader->getId(),
        type,
        selectedStock.getSymbol(),
        price,
        quantity
    );

    cout << "\n========== ORDER CREATED ==========" << endl;

    cout << "Order ID: " << order.id << endl;
    cout << "Trader ID: " << order.traderId << endl;
    cout << "Trader Name: "
         << selectedTrader->getName() << endl;
    cout << "Stock: " << order.symbol << endl;
    cout << "Order Type: " << order.type << endl;
    cout << "Price: Rs. " << order.price << endl;
    cout << "Quantity: " << order.qty << endl;

    return 0;
}