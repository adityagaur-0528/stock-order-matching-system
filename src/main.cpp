#include <iostream>
#include "InputStream.h"
#include "OrderBook.h"
#include "MatchingEngine.h"

using namespace std;

int main()
{
    OrderBook book;

    int numberOfOrders;

    cout << "========================================" << endl;
    cout << "     STOCK ORDER MATCHING SIMULATOR" << endl;
    cout << "========================================" << endl;

    cout << "\nEnter number of orders: ";
    cin >> numberOfOrders;

    if (numberOfOrders <= 0)
    {
        cout << "Invalid number of orders." << endl;
        return 0;
    }

    // Input and validation
    for (int i = 0; i < numberOfOrders; i++)
    {
        cout << "\n---------- Order " << i + 1
             << " ----------" << endl;

        Order order = createOrder();

        if (!validateOrder(order))
        {
            cout << "Order rejected." << endl;
            continue;
        }

        if (book.addOrder(order))
        {
            cout << "Order added to Order Book successfully."
                 << endl;
        }
    }

    // Display orders before matching
    cout << "\n\n========== BEFORE MATCHING ==========";
    book.displayOrderBook();

    // Matching process
    cout << "\n\n========== MATCHING PROCESS ==========" << endl;

    matchOrders(book);

    // Display remaining orders
    cout << "\n\n========== AFTER MATCHING ==========";
    book.displayOrderBook();

    cout << "\n\n========================================" << endl;
    cout << "        SIMULATION COMPLETED" << endl;
    cout << "========================================" << endl;

    return 0;
}