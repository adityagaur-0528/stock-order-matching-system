# Stock Order Matching Simulator

A C++ based Stock Order Matching Simulator designed to simulate the processing and matching of buy and sell orders using Data Structures and Object-Oriented Programming.

## 📌 Project Overview

The system accepts stock buy and sell orders from users and processes them according to price-time priority. It maintains active orders, checks whether compatible buy and sell orders can be matched, and updates the remaining quantities after execution.

## 🎯 Objectives

- Accept and manage buy and sell orders.
- Organize orders based on price priority.
- Implement price-time priority for order matching.
- Handle partial and complete order execution.
- Maintain pending orders after unsuccessful or partial matching.
- Demonstrate the practical use of Data Structures and OOP in C++.

## 🛠️ Technology Stack

- **Language:** C++
- **Concepts:** Data Structures, Object-Oriented Programming
- **Data Structures:** Map, Queue / Order-based structures, BST
- **Development Environment:** Visual Studio Code
- **Version Control:** Git & GitHub

## ⚙️ System Workflow

```text
User Input
    ↓
Order Creation
    ↓
Order Book
    ↓
Price-Time Priority
    ↓
Matching Engine
    ↓
Trade Execution
    ↓


# Stock Order Matching Simulator — V2

This redesign intentionally keeps the project academically strong without turning it into a 3,500+ line monolith.

## Architecture

Web authentication + market-data API → Node/Express → line protocol → C++ trading engine → Custom BST + Custom Queue → price-time matching + trade settlement → Node → graphical terminal.

## API choice

The market-data layer uses Indian API (stock.indianapi.in) instead of Twelve Data. It exposes stock search/details, NSE most-active stocks, current NSE/BSE pricing and historical price data. Put the API key in backend/.env as INDIAN_API_KEY.

## C++ responsibilities

- Trader/account model
- Balance and holding validation
- Cash/holding reservation for pending orders
- Order IDs and sequence
- Custom circular Queue for FIFO at one price level
- Custom BST for price levels
- One OrderBook per symbol
- Price-time matching
- Partial fills
- Trade settlement
- Machine-readable order book output for the UI
Quantity Update
    ↓
Pending / Completed Orders
