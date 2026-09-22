#ifndef ORDER_MANAGEMENT_H
#define ORDER_MANAGEMENT_H

#include "Order.h"

void updateOrder(Order& order, int tradedQuantity);

bool isCompleted(const Order& order);

bool isPending(const Order& order);

void displayOrderStatus(const Order& order);

void processOrderUpdate(Order& order, int tradedQuantity);

#endif