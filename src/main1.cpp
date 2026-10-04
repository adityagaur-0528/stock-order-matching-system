#include <iostream>
#include "simulator.h"

using namespace std;

int main()
{
    // --------------------------------------------------------
    // Creating Traders
    // --------------------------------------------------------

    Trader t1("Aditya", 100000);
    Trader t2("Utkarsh", 100000);
    Trader t3("Shivam", 100000);

    // --------------------------------------------------------
    // Array to temporarily store created orders
    // --------------------------------------------------------

    Order orders[100];

    int orderCount = 0;
    int choice;

    cout << "========================================" << endl;
    cout << "     STOCK ORDER MATCHING SIMULATOR" << endl;
    cout << "========================================" << endl;

    // --------------------------------------------------------
    // Main Menu
    // --------------------------------------------------------

    do
    {
        cout << "\n\n========== MAIN MENU ==========" << endl;

        cout << "1. Add Order" << endl;
        cout << "2. View Traders" << endl;
        cout << "3. View Available Stocks" << endl;
        cout << "4. View All Orders" << endl;
        cout << "5. Exit" << endl;

        cout << "\nEnter Choice: ";
        cin >> choice;

        // ====================================================
        // 1. ADD ORDER
        // ====================================================

        if (choice == 1)
        {
            if (orderCount >= 100)
            {
                cout << "\nOrder limit reached." << endl;
                continue;
            }

            // ------------------------------------------------
            // Select Trader
            // ------------------------------------------------

            cout << "\n========== SELECT TRADER ==========" << endl;

            cout << "1. " << t1.getName()
                 << " | ID: " << t1.getId()
                 << " | Balance: Rs. " << t1.getBalance()
                 << endl;

            cout << "2. " << t2.getName()
                 << " | ID: " << t2.getId()
                 << " | Balance: Rs. " << t2.getBalance()
                 << endl;

            cout << "3. " << t3.getName()
                 << " | ID: " << t3.getId()
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
                    continue;
            }

            // ------------------------------------------------
            // Select Stock
            // ------------------------------------------------

            displayAvailableStocks();

            int stockChoice;

            cout << "\nSelect Stock: ";
            cin >> stockChoice;

            Stock selectedStock = getStock(stockChoice);

            if (selectedStock.getSymbol() == "")
            {
                cout << "Invalid stock choice." << endl;
                continue;
            }

            // ------------------------------------------------
            // Order Type
            // ------------------------------------------------

            char type;

            cout << "\nEnter Order Type (B/S): ";
            cin >> type;

            if (type != 'B' && type != 'S')
            {
                cout << "Invalid order type." << endl;
                continue;
            }

            // ------------------------------------------------
            // Price
            // ------------------------------------------------

            int price;

            cout << "Enter Price: ";
            cin >> price;

            if (price <= 0)
            {
                cout << "Price must be greater than 0." << endl;
                continue;
            }

            // ------------------------------------------------
            // Quantity
            // ------------------------------------------------

            int quantity;

            cout << "Enter Quantity: ";
            cin >> quantity;

            if (quantity <= 0)
            {
                cout << "Quantity must be greater than 0." << endl;
                continue;
            }

            // ------------------------------------------------
            // Create Order
            // ------------------------------------------------

            Order newOrder(
                selectedTrader->getId(),
                type,
                selectedStock.getSymbol(),
                price,
                quantity
            );

            // Store order in array
            orders[orderCount] = newOrder;
            orderCount++;

            // ------------------------------------------------
            // Display Created Order
            // ------------------------------------------------

            cout << "\n========== ORDER CREATED ==========" << endl;

            cout << "Order ID: "
                 << newOrder.id << endl;

            cout << "Trader ID: "
                 << newOrder.traderId << endl;

            cout << "Trader Name: "
                 << selectedTrader->getName() << endl;

            cout << "Stock: "
                 << newOrder.symbol << endl;

            cout << "Order Type: "
                 << newOrder.type << endl;

            cout << "Price: Rs. "
                 << newOrder.price << endl;

            cout << "Quantity: "
                 << newOrder.qty << endl;
        }

        // ====================================================
        // 2. VIEW TRADERS
        // ====================================================

        else if (choice == 2)
        {
            cout << "\n========== TRADERS ==========" << endl;

            t1.display();
            t2.display();
            t3.display();
        }

        // ====================================================
        // 3. VIEW STOCKS
        // ====================================================

        else if (choice == 3)
        {
            displayAvailableStocks();
        }

        // ====================================================
        // 4. VIEW ALL ORDERS
        // ====================================================

        else if (choice == 4)
        {
            if (orderCount == 0)
            {
                cout << "\nNo orders have been created yet."
                     << endl;
                continue;
            }

            cout << "\n========== ALL ORDERS ==========" << endl;

            for (int i = 0; i < orderCount; i++)
            {
                cout << "\nOrder ID: "
                     << orders[i].id << endl;

                cout << "Trader ID: "
                     << orders[i].traderId << endl;

                cout << "Stock: "
                     << orders[i].symbol << endl;

                cout << "Type: "
                     << orders[i].type << endl;

                cout << "Price: Rs. "
                     << orders[i].price << endl;

                cout << "Quantity: "
                     << orders[i].qty << endl;
            }
        }

        // ====================================================
        // 5. EXIT
        // ====================================================

        else if (choice == 5)
        {
            cout << "\nExiting simulator..." << endl;
        }

        // ====================================================
        // INVALID CHOICE
        // ====================================================

        else
        {
            cout << "\nInvalid menu choice." << endl;
        }

    }
    while (choice != 5);

    cout << "\n========================================" << endl;
    cout << "       SIMULATOR CLOSED" << endl;
    cout << "========================================" << endl;

    return 0;
}