#include <iostream>
using namespace std;

struct Order
{
    int id;
    char type;
    int price;
    int qty;
};

int main()
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

    if ((o.type == 'B' || o.type == 'S') &&
        o.price > 0 && o.qty > 0)
    {
        cout << "\nValid Order" << endl;
        cout << "Order forwarded for processing." << endl;
    }
    else
    {
        cout << "\nInvalid Order" << endl;
    }

    return 0;
}
