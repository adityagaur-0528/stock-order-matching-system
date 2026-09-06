#include <iostream>
using namespace std;

struct Order
{
    int id;
    int qty;
    bool active;
};

void updateOrder(Order &o, int traded)
{
    o.qty -= traded;

    if (o.qty == 0)
        o.active = false;
}

void display(Order o)
{
    cout << "Order ID: " << o.id << endl;
    cout << "Remaining Quantity: " << o.qty << endl;

    if (o.active)
        cout << "Status: Pending" << endl;
    else
        cout << "Status: Completed" << endl;
}

int main()
{
    Order o = {101, 20, true};

    cout << "Before Trade:\n";
    display(o);

    updateOrder(o, 8);

    cout << "\nAfter Trade:\n";
    display(o);

    return 0;
}