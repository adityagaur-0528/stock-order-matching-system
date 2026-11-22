# Stock Order Matching Simulator — V2

This redesign intentionally keeps the project academically strong without turning it into a 3,500+ line monolith.

## Architecture

Web authentication + market-data API → Node/Express → line protocol → C++ trading engine → Custom BST + Custom Queue → price-time matching + trade settlement → Node → graphical terminal.

## API choice

The market-data layer uses Indian API (`stock.indianapi.in`) instead of Twelve Data. It exposes stock search/details, NSE most-active stocks, current NSE/BSE pricing and historical price data. Put the API key in `backend/.env` as `INDIAN_API_KEY`.

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

## Web responsibilities

- Password hashing with Node's `crypto.scrypt`
- Token-based authenticated sessions
- User → Trader ID mapping
- Market-data integration
- API boundary validation
- Graphical trading terminal
- Market-depth visualization
- Price chart

## No DBMS

User accounts are persisted as a small JSON file because the project does not require a database. Passwords are never stored in plaintext.

## Build (PowerShell)

From the `cpp` folder:

```powershell
g++ -static -std=c++17 .\models.cpp .\data_structures.cpp .\trading_engine.cpp .\engine_bridge.cpp -o .\engine_bridge.exe
```

Then from `backend`:

```powershell
npm install
copy .env.example .env
# put your Indian API key in .env
node server.js
```

Open `http://localhost:5000/login.html`.
