#include <iostream>
#include "simulator.h"

using namespace std;

int main()
{
    displayAvailableStocks();

    int choice;

    cout << "\nEnter stock choice: ";
    cin >> choice;

    Stock stock = getStock(choice);

    if (stock.getSymbol() == "")
    {
        cout << "Invalid stock choice." << endl;
        return 0;
    }

    cout << "\nSelected Stock:" << endl;

    stock.display();

    return 0;
}