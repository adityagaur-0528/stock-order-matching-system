#include <iostream>
#include <string>
#include "simulator.h"

using namespace std;

int main()
{
    // --------------------------------------------------------
    // Maximum Traders
    // --------------------------------------------------------

    Trader traders[20];

    int traderCount = 0;


    // --------------------------------------------------------
    // Temporary Order Storage
    // Custom Queue/BST will replace this later.
    // --------------------------------------------------------

    Order orders[100];

    int orderCount = 0;

    int choice;


    // --------------------------------------------------------
    // Welcome
    // --------------------------------------------------------

    cout << "========================================"
         << endl;

    cout << "     STOCK ORDER MATCHING SIMULATOR"
         << endl;

    cout << "========================================"
         << endl;


    // --------------------------------------------------------
    // Main Menu
    // --------------------------------------------------------

    do
    {
        cout << "\n\n========== MAIN MENU =========="
             << endl;

        cout << "1. Create Trader" << endl;
        cout << "2. Add Stock Holding" << endl;
        cout << "3. Place Order" << endl;
        cout << "4. View Traders" << endl;
        cout << "5. View Available Stocks" << endl;
        cout << "6. View All Orders" << endl;
        cout << "7. Match Orders" << endl;
        cout << "8. View Holdings" << endl;
        cout << "9. Exit" << endl;


        cout << "\nEnter Choice: ";
        cin >> choice;


        // ====================================================
        // 1. CREATE TRADER
        // ====================================================

        if (choice == 1)
        {
            if (traderCount >= 20)
            {
                cout
                    << "\nMaximum trader limit reached."
                    << endl;

                continue;
            }


            string name;

            double balance;


            cout << "\nEnter Trader Name: ";
            cin >> name;


            cout << "Enter Initial Balance: Rs. ";
            cin >> balance;


            if (balance <= 0)
            {
                cout
                    << "Balance must be greater than 0."
                    << endl;

                continue;
            }


            // Create new Trader
            traders[traderCount] =
                Trader(name, balance);


            cout
                << "\n========== TRADER CREATED =========="
                << endl;

            cout
                << "Name: "
                << traders[traderCount].getName()
                << endl;

            cout
                << "Trader ID: "
                << traders[traderCount].getId()
                << endl;

            cout
                << "Balance: Rs. "
                << traders[traderCount].getBalance()
                << endl;


            traderCount++;
        }


        // ====================================================
        // 2. ADD STOCK HOLDING
        // ====================================================

        else if (choice == 2)
        {
            if (traderCount == 0)
            {
                cout
                    << "\nNo traders available."
                    << endl;

                continue;
            }


            cout
                << "\n========== SELECT TRADER =========="
                << endl;


            for (int i = 0;
                 i < traderCount;
                 i++)
            {
                cout
                    << i + 1
                    << ". "
                    << traders[i].getName()
                    << " | ID: "
                    << traders[i].getId()
                    << endl;
            }


            int traderChoice;

            cout << "\nSelect Trader: ";
            cin >> traderChoice;


            if (traderChoice < 1 ||
                traderChoice > traderCount)
            {
                cout
                    << "Invalid trader choice."
                    << endl;

                continue;
            }


            Trader& selectedTrader =
                traders[traderChoice - 1];


            // ------------------------------------------------
            // Select Stock
            // ------------------------------------------------

            displayAvailableStocks();


            int stockChoice;

            cout << "\nSelect Stock: ";
            cin >> stockChoice;


            Stock selectedStock =
                getStock(stockChoice);


            if (selectedStock.getSymbol() == "")
            {
                cout
                    << "Invalid stock choice."
                    << endl;

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
                cout
                    << "Quantity must be greater than 0."
                    << endl;

                continue;
            }


            
            if (!selectedTrader.addHolding(
                    selectedStock.getSymbol(),
                    quantity))
            {
                cout
                    << "\nCould not add holding."
                    << endl;

                continue;
            }


            cout
                << "\nHolding added successfully."
                << endl;

            cout
                << "Trader: "
                << selectedTrader.getName()
                << endl;

            cout
                << "Stock: "
                << selectedStock.getSymbol()
                << endl;

            cout
                << "Quantity: "
                << selectedTrader.getHoldingQuantity(
                       selectedStock.getSymbol())
                << endl;
        }


        // ====================================================
        // 3. PLACE ORDER
        // ====================================================

        else if (choice == 3)
        {
            if (traderCount == 0)
            {
                cout
                    << "\nNo traders available."
                    << endl;

                continue;
            }


            if (orderCount >= 100)
            {
                cout
                    << "\nOrder limit reached."
                    << endl;

                continue;
            }


            // ------------------------------------------------
            // Select Trader
            // ------------------------------------------------

            cout
                << "\n========== SELECT TRADER =========="
                << endl;


            for (int i = 0;
                 i < traderCount;
                 i++)
            {
                cout
                    << i + 1
                    << ". "
                    << traders[i].getName()
                    << " | ID: "
                    << traders[i].getId()
                    << " | Balance: Rs. "
                    << traders[i].getBalance()
                    << endl;
            }


            int traderChoice;

            cout << "\nSelect Trader: ";
            cin >> traderChoice;


            if (traderChoice < 1 ||
                traderChoice > traderCount)
            {
                cout
                    << "Invalid trader choice."
                    << endl;

                continue;
            }


            Trader& selectedTrader =
                traders[traderChoice - 1];


            // ------------------------------------------------
            // Select Stock
            // ------------------------------------------------

            displayAvailableStocks();


            int stockChoice;

            cout << "\nSelect Stock: ";
            cin >> stockChoice;


            Stock selectedStock =
                getStock(stockChoice);


            if (selectedStock.getSymbol() == "")
            {
                cout
                    << "Invalid stock choice."
                    << endl;

                continue;
            }


            // ------------------------------------------------
            // Order Type
            // ------------------------------------------------

            char type;

            cout
                << "\nEnter Order Type (B/S): ";

            cin >> type;


            if (type == 'b')
            {
                type = 'B';
            }

            if (type == 's')
            {
                type = 'S';
            }


            if (type != 'B' &&
                type != 'S')
            {
                cout
                    << "Invalid order type."
                    << endl;

                continue;
            }


            // ------------------------------------------------
            // Price
            // ------------------------------------------------

            int price;

            cout << "Enter Price: ";
            cin >> price;


            // IMPORTANT:
            // Order price must exactly match
            // the predefined simulated stock price.

            int fixedPrice =
                static_cast<int>(
                    selectedStock.getCurrentPrice()
                );


            if (price != fixedPrice)
            {
                cout
                    << "\nOrder rejected."
                    << endl;

                cout
                    << "Current simulated price of "
                    << selectedStock.getSymbol()
                    << " is Rs. "
                    << fixedPrice
                    << endl;

                cout
                    << "You must enter exactly Rs. "
                    << fixedPrice
                    << endl;

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
                cout
                    << "Quantity must be greater than 0."
                    << endl;

                continue;
            }


            // ------------------------------------------------
            // BUY Validation
            // ------------------------------------------------

            if (type == 'B')
            {
                if (!selectedTrader.canBuy(
                        price,
                        quantity))
                {
                    double requiredAmount =
                        static_cast<double>(price)
                        * quantity;


                    cout
                        << "\nOrder rejected."
                        << endl;

                    cout
                        << "Insufficient balance."
                        << endl;

                    cout
                        << "Required: Rs. "
                        << requiredAmount
                        << endl;

                    cout
                        << "Available: Rs. "
                        << selectedTrader.getBalance()
                        << endl;

                    continue;
                }
            }


            // ------------------------------------------------
            // SELL Validation
            // ------------------------------------------------

            else
            {
                int availableQuantity =
                    selectedTrader.getHoldingQuantity(
                        selectedStock.getSymbol()
                    );


                if (availableQuantity < quantity)
                {
                    cout
                        << "\nOrder rejected."
                        << endl;

                    cout
                        << "Insufficient holdings."
                        << endl;

                    cout
                        << "Available "
                        << selectedStock.getSymbol()
                        << ": "
                        << availableQuantity
                        << endl;

                    continue;
                }
            }


            // ------------------------------------------------
            // Create Order
            // ------------------------------------------------

            Order newOrder(
                selectedTrader.getId(),
                type,
                selectedStock.getSymbol(),
                price,
                quantity
            );


            orders[orderCount] =
                newOrder;

            orderCount++;


            // ------------------------------------------------
            // Display Created Order
            // ------------------------------------------------

            cout
                << "\n========== ORDER CREATED =========="
                << endl;

            cout
                << "Order ID: "
                << newOrder.id
                << endl;

            cout
                << "Trader: "
                << selectedTrader.getName()
                << endl;

            cout
                << "Trader ID: "
                << selectedTrader.getId()
                << endl;

            cout
                << "Stock: "
                << newOrder.symbol
                << endl;

            cout
                << "Type: "
                << newOrder.type
                << endl;

            cout
                << "Price: Rs. "
                << newOrder.price
                << endl;

            cout
                << "Quantity: "
                << newOrder.qty
                << endl;
        }


        // ====================================================
        // 4. VIEW TRADERS
        // ====================================================

        else if (choice == 4)
        {
            if (traderCount == 0)
            {
                cout
                    << "\nNo traders created yet."
                    << endl;

                continue;
            }


            cout
                << "\n========== TRADERS =========="
                << endl;


            for (int i = 0;
                 i < traderCount;
                 i++)
            {
                traders[i].display();
            }
        }


        // ====================================================
        // 5. VIEW AVAILABLE STOCKS
        // ====================================================

        else if (choice == 5)
        {
            displayAvailableStocks();
        }


        // ====================================================
        // 6. VIEW ALL ORDERS
        // ====================================================

        else if (choice == 6)
        {
            if (orderCount == 0)
            {
                cout
                    << "\nNo orders have been created yet."
                    << endl;

                continue;
            }


            cout
                << "\n========== ALL ORDERS =========="
                << endl;


            for (int i = 0;
                 i < orderCount;
                 i++)
            {
                Trader* trader =
                    findTraderById(
                        orders[i].traderId,
                        traders,
                        traderCount
                    );


                cout
                    << "\nOrder ID: "
                    << orders[i].id
                    << endl;

                cout
                    << "Trader: "
                    << (trader != nullptr
                            ? trader->getName()
                            : "Unknown")
                    << endl;

                cout
                    << "Trader ID: "
                    << orders[i].traderId
                    << endl;

                cout
                    << "Stock: "
                    << orders[i].symbol
                    << endl;

                cout
                    << "Type: "
                    << orders[i].type
                    << endl;

                cout
                    << "Price: Rs. "
                    << orders[i].price
                    << endl;

                cout
                    << "Remaining Quantity: "
                    << orders[i].qty
                    << endl;
            }
        }


        // ====================================================
        // 7. MATCH ORDERS
        // ====================================================

        else if (choice == 7)
        {
            if (orderCount < 2)
            {
                cout
                    << "\nNot enough orders to match."
                    << endl;

                continue;
            }


            matchOrders(
                orders,
                orderCount,
                traders,
                traderCount
            );
        }


        // ====================================================
        // 8. VIEW HOLDINGS
        // ====================================================

        else if (choice == 8)
        {
            if (traderCount == 0)
            {
                cout
                    << "\nNo traders created yet."
                    << endl;

                continue;
            }


            cout
                << "\n========== TRADER HOLDINGS =========="
                << endl;


            for (int i = 0;
                 i < traderCount;
                 i++)
            {
                traders[i].displayHoldings();
            }
        }


        // ====================================================
        // 9. EXIT
        // ====================================================

        else if (choice == 9)
        {
            cout
                << "\nExiting simulator..."
                << endl;
        }


        // ====================================================
        // INVALID CHOICE
        // ====================================================

        else
        {
            cout
                << "\nInvalid menu choice."
                << endl;
        }

    }
    while (choice != 9);


    cout
        << "\n========================================"
        << endl;

    cout
        << "       SIMULATOR CLOSED"
        << endl;

    cout
        << "========================================"
        << endl;


    return 0;
}
