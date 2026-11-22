#include "data_structures.h"
#include <iomanip>
using namespace std;
CustomQueue::CustomQueue()
    : front(0), rear(-1), count(0) {}
bool CustomQueue::enqueue(const Order& order)
{
    if (isFull()) return false;

    rear = (rear + 1) % MAX_SIZE;
    orders[rear] = order;
    ++count;

    return true;
}
bool CustomQueue::dequeue()
{
    if (isEmpty()) return false;

    front = (front + 1) % MAX_SIZE;
    --count;

    if (count == 0)
    {
        front = 0;
        rear = -1;
    }
    return true;
}
Order& CustomQueue::getFront()
{
    return orders[front];
}

const Order& CustomQueue::getFront() const
{
    return orders[front];
}
const Order& CustomQueue::getAt(int index) const
{
    return orders[(front + index) % MAX_SIZE];
}
bool CustomQueue::isEmpty() const
{
    return count == 0;
}

bool CustomQueue::isFull() const
{
    return count == MAX_SIZE;
}

int CustomQueue::size() const
{
    return count;
}

bool CustomQueue::removeById(int orderId)
{
    if (isEmpty()) return false;

    Order temp[MAX_SIZE];
    int newCount = 0;
    bool removed = false;

    for (int i = 0; i < count; ++i)
    {
        const Order& current = getAt(i);

        if (current.id == orderId)
            removed = true;
        else
            temp[newCount++] = current;
    }

    if (!removed) return false;

    front = 0;
    rear = -1;
    count = 0;

    for (int i = 0; i < newCount; ++i)
        enqueue(temp[i]);

    return true;
}

void CustomQueue::display(ostream& out) const
{
    for (int i = 0; i < count; ++i)
    {
        const Order& order = getAt(i);

        out << "ORDER|" << order.id << "|" << order.traderId << "|"
            << order.qty << "|" << fixed << setprecision(2)
            << order.price << "\n";
    }
}
BSTNode::BSTNode()
    : price(0.0), left(-1), right(-1), active(false) {}

CustomBST::CustomBST()
    : root(-1), nodeCount(0) {}

int CustomBST::createNode(double price)
{
    if (nodeCount >= MAX_NODES) return -1;

    for (int i = 0; i < MAX_NODES; ++i)
    {
        if (!nodes[i].active)
        {
            nodes[i].price = price;
            nodes[i].left = -1;
            nodes[i].right = -1;
            nodes[i].active = true;
            ++nodeCount;
            return i;
        }
    }

    return -1;
}

int CustomBST::insertNode(int current, const Order& order, bool& inserted)
{
    if (current == -1)
    {
        int newNode = createNode(order.price);

        if (newNode == -1)
        {
            inserted = false;
            return -1;
        }

        if (!nodes[newNode].orders.enqueue(order))
        {
            nodes[newNode].active = false;
            --nodeCount;
            inserted = false;
            return -1;
        }

        inserted = true;
        return newNode;
    }

    if (order.price == nodes[current].price)
    {
        inserted = nodes[current].orders.enqueue(order);
        return current;
    }

    if (order.price < nodes[current].price)
    {
        int child = insertNode(nodes[current].left, order, inserted);
        if (inserted) nodes[current].left = child;
    }
    else
    {
        int child = insertNode(nodes[current].right, order, inserted);
        if (inserted) nodes[current].right = child;
    }

    return current;
}

bool CustomBST::insert(const Order& order)
{
    bool inserted = false;
    int newRoot = insertNode(root, order, inserted);

    if (root == -1 && inserted)
        root = newRoot;

    return inserted;
}
int CustomBST::findMinIndex(int current) const
{
    if (current == -1)
        return -1;

    while (nodes[current].left != -1)
    {
        current = nodes[current].left;
    }

    return current;
}
int CustomBST::findMaxIndex(int current) const
{
    if (current == -1)
        return -1;

    while (nodes[current].right != -1)
    {
        current = nodes[current].right;
    }

    return current;
}
Order& CustomBST::getMinOrder()
{
    return nodes[findMinIndex(root)].orders.getFront();
}

Order& CustomBST::getMaxOrder()
{
    return nodes[findMaxIndex(root)].orders.getFront();
}
const Order& CustomBST::getMinOrder() const
{
    return nodes[findMinIndex(root)].orders.getFront();
}

const Order& CustomBST::getMaxOrder() const
{
    return nodes[findMaxIndex(root)].orders.getFront();
}
int CustomBST::removeMinNode(int current)
{
    if (current == -1)
        return -1;

    if (nodes[current].left == -1)
    {
        int rightChild = nodes[current].right;

        nodes[current].active = false;
        nodes[current].left = -1;
        nodes[current].right = -1;

        --nodeCount;
        return rightChild;
    }

    nodes[current].left =
        removeMinNode(nodes[current].left);

    return current;
}
int CustomBST::removeMaxNode(int current)
{
    if (current == -1)
        return -1;

    if (nodes[current].right == -1)
    {
        int leftChild = nodes[current].left;

        nodes[current].active = false;
        nodes[current].left = -1;
        nodes[current].right = -1;

        --nodeCount;
        return leftChild;
    }

    nodes[current].right =
        removeMaxNode(nodes[current].right);

    return current;
}
bool CustomBST::removeMinOrder()
{
    if (isEmpty())
        return false;

    int index = findMinIndex(root);

    if (nodes[index].orders.size() > 1)
        return nodes[index].orders.dequeue();

    root = removeMinNode(root);
    return true;
}

bool CustomBST::removeMaxOrder()
{
    if (isEmpty())
        return false;

    int index = findMaxIndex(root);

    if (nodes[index].orders.size() > 1)
        return nodes[index].orders.dequeue();

    root = removeMaxNode(root);
    return true;
}
bool CustomBST::cancelNode(int current, int orderId, bool& removed)
{
    if (current == -1 || removed)
        return current;

    if (nodes[current].orders.removeById(orderId))
    {
        removed = true;

        if (nodes[current].orders.isEmpty())
        {
            if (nodes[current].left == -1)
            {
                int rightChild = nodes[current].right;
                nodes[current].active = false;
                --nodeCount;
                return rightChild;
            }

            if (nodes[current].right == -1)
            {
                int leftChild = nodes[current].left;
                nodes[current].active = false;
                --nodeCount;
                return leftChild;
            }

            int successor = findMinIndex(nodes[current].right);

            nodes[current].price = nodes[successor].price;
            nodes[current].orders = nodes[successor].orders;

            nodes[current].right =
                removeMinNode(nodes[current].right);
        }

        return current;
    }

    nodes[current].left =
        cancelNode(nodes[current].left, orderId, removed);

    if (!removed)
    {
        nodes[current].right =
            cancelNode(nodes[current].right, orderId, removed);
    }

    return current;
}
bool CustomBST::cancelOrder(int orderId)
{
    bool removed = false;

    root = cancelNode(root, orderId, removed);
    // A deleted root slot must never remain as the tree root. This defensive
    // check also protects traversal if a previously freed slot is encountered.
    if (root >= 0 && !nodes[root].active)
        root = -1;

    return removed;
}

bool CustomBST::getOrderById(int orderId, Order& result) const
{
    // The tree is small and fixed-capacity; scan active price levels while
    // preserving the queue's price-time order.
    for (int i = 0; i < MAX_NODES; ++i)
    {
        if (!nodes[i].active) continue;
        for (int j = 0; j < nodes[i].orders.size(); ++j)
        {
            const Order& order = nodes[i].orders.getAt(j);
            if (order.id == orderId)
            {
                result = order;
                return true;
            }
        }
    }
    return false;
}
bool CustomBST::isEmpty() const
{
    return root == -1;
}

int CustomBST::size() const
{
    return nodeCount;
}
void CustomBST::emitAscending(int current, ostream& out) const
{
    if (current == -1 || !nodes[current].active)
        return;

    emitAscending(nodes[current].left, out);

    out << "LEVEL|" << fixed << setprecision(2)
        << nodes[current].price
        << "|" << nodes[current].orders.size() << "\n";

    nodes[current].orders.display(out);

    emitAscending(nodes[current].right, out);
}

void CustomBST::emitAscending(ostream& out) const
{
    emitAscending(root, out);
}
void CustomBST::emitDescending(int current, ostream& out) const
{
    if (current == -1 || !nodes[current].active)
        return;

    emitDescending(nodes[current].right, out);

    out << "LEVEL|" << fixed << setprecision(2)
        << nodes[current].price
        << "|" << nodes[current].orders.size() << "\n";

    nodes[current].orders.display(out);

    emitDescending(nodes[current].left, out);
}

void CustomBST::emitDescending(ostream& out) const
{
    emitDescending(root, out);
}