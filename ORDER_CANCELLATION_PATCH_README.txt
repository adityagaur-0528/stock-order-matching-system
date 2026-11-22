ORDERS CANCELLATION PATCH - MARKETSIM V2

WHAT CHANGED
- Added C++ cancellation lookup and cancellation handling.
- Enforced that a trader can cancel only their own pending order.
- Releases reserved cash/shares only for the still-open quantity.
- Added authenticated POST /api/order/:id/cancel endpoint.
- Updates order history to CANCELLED and refreshes the account.
- Added Cancel buttons to My Orders for OPEN and PARTIAL orders.
- Added guard checks to avoid displaying inactive BST nodes after cancellation.

IMPORTANT: REBUILD THE C++ ENGINE ON WINDOWS
The existing engine_bridge.exe in this ZIP is the old prebuilt executable. Rebuild it once so it includes the new C++ cancellation implementation.

1. Open PowerShell or the VS Code terminal.
2. Change directory to StockOrderMatchingSimulator_V2\cpp.
3. Run:
   g++ -static -std=c++17 .\models.cpp .\data_structures.cpp .\trading_engine.cpp .\engine_bridge.cpp -o .\engine_bridge.exe
4. Return to the backend folder and start the server as usual:
   node server.js
5. Open the app and sign in. Pending orders should now show a Cancel action.

Do not overwrite backend/.env or backend/data/users.json with another project's copies.

BEHAVIOUR
- OPEN and PARTIAL orders can be cancelled.
- FILLED, already-cancelled, and missing orders cannot be cancelled.
- Only the order owner can cancel an order.
- Completed trades remain in history; only the unfilled remainder is cancelled.
- Cancelled orders are retained in history with status CANCELLED.

VALIDATION PERFORMED
- C++ engine compiled successfully with -Wall -Wextra -pedantic.
- Smoke-tested order cancellation and release of reserved cash.
- Tested ownership rejection and partial-fill cancellation.
- Node syntax checks passed for backend/server.js and frontend/app.js.

UI clarification update:
- The My Orders table now labels the execution column as "Execution".
- A partially filled cancelled order is shown as, for example, "2 filled · 2 cancelled" and status "CANCELLED · PARTIALLY FILLED".
- A fully cancelled unfilled order is shown as "0 filled · N cancelled" and status "CANCELLED".
