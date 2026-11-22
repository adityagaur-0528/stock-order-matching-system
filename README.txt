MarketSim starter portfolio update
==================================

What this update adds
- Every new trader receives exactly 25 distinct starter stock symbols.
- Each starter holding receives a random quantity from 5 through 30 shares.
- Starter shares are virtual and free; cash starts at exactly ₹100,000.
- Initial portfolio is saved in backend/data/users.json and restored into the C++ engine after a backend restart.
- BUY orders reserve sufficient virtual cash; SELL orders reserve shares.
- Filled trades settle buyer cash/shares and seller cash/shares in the C++ engine.
- Backend serializes commands sent to the single C++ process so simultaneous requests do not collide.
- After a fill, backend refreshes both counterparties' account balances/holdings and records the trade for both users.
- Existing users are migrated once to a starter portfolio. Because the C++ order book is in memory, open/partial orders from a previous server run are marked CANCELLED_RESTART.

Files in this update
- backend/server.js
- cpp/trading_engine.cpp
- cpp/trading_engine.h
- cpp/engine_bridge.cpp
- cpp/models.h
- cpp/data_structures.h
- frontend/app.js
- frontend/auth.js
- frontend/index.html
- frontend/login.html
- frontend/market-sim-overrides.css

Integration
1. Make a backup or commit your current working project before replacing files.
2. Copy these folders over the matching backend/, cpp/, and frontend/ folders in your project.
3. Keep your existing cpp/models.cpp and cpp/data_structures.cpp; their implementations are still used.
4. Ensure backend/.env contains INDIAN_API_KEY if you want market search/quotes/charts. Portfolio seeding itself does not need the market API.
5. From the project root, compile the C++ engine (Windows MinGW example):
   g++ -std=c++17 .\cpp\engine_bridge.cpp .\cpp\trading_engine.cpp .\cpp\data_structures.cpp .\cpp\models.cpp -o .\cpp\engine_bridge.exe
6. Start your backend using your existing npm start command.

Notes
- This is still a no-DBMS demo: accounts and history are stored in backend/data/users.json; sessions are in memory and users must log in again after a backend restart.
- The shared order book is live while the C++ engine process is running; it is not persisted across restarts.
- Starter stocks are selected from a built-in list of NSE-style ticker symbols. Market quote availability depends on the configured provider/API plan; the simulator does not fabricate prices.
- The C++ engine source passed a C++17 syntax-only check against the supplied interfaces, and the JavaScript files passed node --check. Run the full local build above against your current data_structures.cpp/models.cpp before testing trades.
