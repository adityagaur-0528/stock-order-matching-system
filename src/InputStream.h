#ifndef INPUT_STREAM_H
#define INPUT_STREAM_H

#include "Order.h"

Order createOrder();
bool validateOrder(const Order& order);

#endif