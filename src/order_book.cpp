#include <iostream>
#include <map>
#include <queue>
using namespace std;

class Order
{
public:
    int id;
    char type;
    int price;
    int qty;

    Order(int i, char t, int p, int q)
    {
        id = i;
        type = t;
        price = p;
        qty = q;
    }
};

int main()
{
    // Highest price first for Buy orders
    map<int, queue<Order>, greater<int>> buy;

    // Lowest price first for Sell orders
    map<int, queue<Order>> sell;

    // Sample orders
    buy[105].push(Order(1, 'B', 105, 10));
    buy[103].push(Order(2, 'B', 103, 5));

    sell[103].push(Order(3, 'S', 103, 6));
    sell[108].push(Order(4, 'S', 108, 10));

    // Matching
    while (!buy.empty() && !sell.empty())
    {
        auto b = buy.begin();
        auto s = sell.begin();

        if (b->first >= s->first)
        {
            Order &bo = b->second.front();
            Order &so = s->second.front();

            int trade = min(bo.qty, so.qty);

            cout << "Trade: " << trade
                 << " shares at Rs. " << s->first << endl;

            bo.qty -= trade;
            so.qty -= trade;

            if (bo.qty == 0)
                b->second.pop();

            if (so.qty == 0)
                s->second.pop();

            if (b->second.empty())
                buy.erase(b);

            if (s->second.empty())
                sell.erase(s);
        }
        else
        {
            break;
        }
    }

    return 0;
}