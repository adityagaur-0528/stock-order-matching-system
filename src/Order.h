#ifndef ORDER_H
#define ORDER_H

class Order
{
public:
    int id;
    char type;
    int price;
    int qty;

    Order()
    {
        id = 0;
        type = ' ';
        price = 0;
        qty = 0;
    }

    Order(int i, char t, int p, int q)
    {
        id = i;
        type = t;
        price = p;
        qty = q;
    }
};

#endif