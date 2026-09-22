#include <iostream>
#include "OrderManagement.h"

using namespace std;

void updateOrder(Order& order, int tradedQuantity)
{
    if (tradedQuantity <= 0)
    {
        cout << "Invalid traded quantity." << endl;
        return;
    }

    if (tradedQuantity > order.qty)
    {
        cout << "Traded quantity cannot exceed order quantity."
             << endl;
        return;
    }

    order.qty -= tradedQuantity;

    cout << "\nOrder Updated" << endl;
    cout << "Order ID: " << order.id << endl;
    cout << "Traded Quantity: " << tradedQuantity << endl;
    cout << "Remaining Quantity: " << order.qty << endl;
}

bool isCompleted(const Order& order)
{
    return order.qty == 0;
}

bool isPending(const Order& order)
{
    return order.qty > 0;
}

void displayOrderStatus(const Order& order)
{
    cout << "\n---------- Order Status ----------" << endl;
    cout << "Order ID: " << order.id << endl;
    cout << "Order Type: " << order.type << endl;
    cout << "Price: " << order.price << endl;
    cout << "Remaining Quantity: " << order.qty << endl;

    if (isCompleted(order))
    {
        cout << "Status: Completed" << endl;
    }
    else
    {
        cout << "Status: Pending" << endl;
    }
}

void processOrderUpdate(Order& order, int tradedQuantity)
{
    updateOrder(order, tradedQuantity);

    if (isCompleted(order))
    {
        cout << "Order " << order.id
             << " has been fully executed." << endl;
    }
    else
    {
        cout << "Order " << order.id
             << " remains pending." << endl;
    }
}