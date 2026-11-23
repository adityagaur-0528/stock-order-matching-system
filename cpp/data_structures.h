#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "models.h"
#include <ostream>

using namespace std;

class CustomQueue
{
    static const int MAX_SIZE = 64;

    Order orders[MAX_SIZE];
    int front;
    int rear;
    int count;

public:
    CustomQueue();

    bool enqueue(const Order&);
    bool dequeue();

    Order& getFront();
    const Order& getFront() const;

    const Order& getAt(int) const;

    bool isEmpty() const;
    bool isFull() const;
    int size() const;

    bool removeById(int);
    void display(ostream&) const;
};

class BSTNode
{
public:
    double price;
    CustomQueue orders;
    int left;
    int right;
    bool active;

    BSTNode();
};

class CustomBST
{
    static const int MAX_NODES = 64;

    BSTNode nodes[MAX_NODES];
    int root;
    int nodeCount;

    int createNode(double);
    int insertNode(int, const Order&, bool&);
    int findMinIndex(int) const;
    int findMaxIndex(int) const;

    int removeMinNode(int);
    int removeMaxNode(int);

    int cancelNode(int, int, bool&);

    void emitAscending(int, ostream&) const;
    void emitDescending(int, ostream&) const;

public:
    CustomBST();

    bool insert(const Order&);

    Order& getMinOrder();
    Order& getMaxOrder();

    const Order& getMinOrder() const;
    const Order& getMaxOrder() const;

    bool removeMinOrder();
    bool removeMaxOrder();
    bool cancelOrder(int);
    bool getOrderById(int, Order&) const;

    bool isEmpty() const;
    int size() const;

    void emitAscending(ostream&) const;
    void emitDescending(ostream&) const;
};

#endif