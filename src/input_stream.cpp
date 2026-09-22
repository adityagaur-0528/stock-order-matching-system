#include <iostream>
#include "InputStream.h"

using namespace std;

bool validateOrder(const Order& o)
{
    if (o.id <= 0)
    {
        cout << "Invalid Order ID." << endl;
        return false;
    }

    if (o.type != 'B' && o.type != 'S')
    {
        cout << "Invalid Order Type. Use B or S." << endl;
        return false;
    }

    if (o.price <= 0)
    {
        cout << "Price must be greater than 0." << endl;
        return false;
    }

    if (o.qty <= 0)
    {
        cout << "Quantity must be greater than 0." << endl;
        return false;
    }

    return true;
}

Order createOrder()
{
    Order o;

    cout << "Enter Order ID: ";
    cin >> o.id;

    cout << "Enter Order Type (B/S): ";
    cin >> o.type;

    cout << "Enter Price: ";
    cin >> o.price;

    cout << "Enter Quantity: ";
    cin >> o.qty;

    return o;
}