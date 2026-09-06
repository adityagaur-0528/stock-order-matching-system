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
Quantity Update
    ↓
Pending / Completed Orders