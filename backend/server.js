const express = require("express");
const path = require("path");
const dotenv = require("dotenv");
const crypto = require("crypto");
const fs = require("fs");
const { spawn } = require("child_process");

dotenv.config({ path: path.join(__dirname, ".env") });

const app = express();
const PORT = Number(process.env.PORT || 5000);
const API_KEY = process.env.INDIAN_API_KEY;
const API_BASE = "https://stock.indianapi.in";
const FRONTEND = path.join(__dirname, "../frontend");
const ENGINE_FILE = process.platform === "win32"
    ? "engine_bridge.exe"
    : "engine_bridge";

const ENGINE_PATH = path.join(__dirname, "../cpp", ENGINE_FILE);
const USERS_FILE = path.join(__dirname, "data/users.json");
fs.mkdirSync(path.dirname(USERS_FILE), { recursive: true });

app.use(express.json());
app.use(express.static(FRONTEND));

const STARTING_CASH = 100000;
const STARTER_STOCKS = [
    "RELIANCE", "TCS", "HDFCBANK", "ICICIBANK", "INFY", "SBIN", "BHARTIARTL",
    "ITC", "LT", "HINDUNILVR", "AXISBANK", "KOTAKBANK", "BAJFINANCE", "MARUTI",
    "SUNPHARMA", "TATAMOTORS", "TITAN", "ASIANPAINT", "NTPC", "POWERGRID", "ONGC",
    "WIPRO", "TECHM", "ULTRACEMCO", "JSWSTEEL", "TATASTEEL", "ADANIPORTS",
    "COALINDIA", "NESTLEIND", "HCLTECH", "INDUSINDBK", "DRREDDY", "CIPLA",
    "EICHERMOT", "GRASIM", "DIVISLAB", "BPCL", "HEROMOTOCO", "APOLLOHOSP", "TRENT"
];

function shuffled(items) {
    const copy = [...items];
    for (let i = copy.length - 1; i > 0; i--) {
        const j = crypto.randomInt(i + 1);
        [copy[i], copy[j]] = [copy[j], copy[i]];
    }
    return copy;
}

function makeStarterPortfolio(existingHoldings = []) {
    const holdings = new Map();
    for (const item of existingHoldings) {
        const symbol = String(item.symbol || "")
            .trim()
            .toUpperCase();
        const quantity = Number(item.quantity);
        if (symbol && Number.isSafeInteger(quantity) && quantity > 0) holdings.set(symbol, quantity);
    }
    for (const symbol of shuffled(STARTER_STOCKS)) {
        if (holdings.size >= 25) break;
        if (!holdings.has(symbol)) holdings.set(symbol, crypto.randomInt(5, 31));
    }
    return [...holdings].map(([symbol, quantity]) => ({ symbol, quantity }));
}

function loadUsers() {
    try {
        return JSON.parse(fs.readFileSync(USERS_FILE, "utf8"));
    } catch {
        return { users: [] };
    }
}

function saveUsers(data) {
    fs.writeFileSync(USERS_FILE, JSON.stringify(data, null, 2));
}

function hashPassword(password, salt = crypto.randomBytes(16).toString("hex")) {
    const hash = crypto.scryptSync(password, salt, 64).toString("hex");
    return { salt, hash };
}

function verifyPassword(password, salt, expectedHash) {
    const actual = crypto.scryptSync(password, salt, 64).toString("hex");
    return crypto.timingSafeEqual(Buffer.from(actual, "hex"), Buffer.from(expectedHash, "hex"));
}

function issueToken(username) {
    const token = crypto.randomBytes(32).toString("hex");
    sessions.set(token, username);
    return token;
}

const sessions = new Map();
const users = loadUsers();

function ensureUserHistory(user) {
    if (!Array.isArray(user.orders)) {
        user.orders = [];
    }
    if (!Array.isArray(user.trades)) {
        user.trades = [];
    }
}

function addOrderToHistory(user, order) {
    ensureUserHistory(user);
    const record = {
        ...order,
        status: "OPEN",
        filledQuantity: 0,
        remainingQuantity: Number(order.quantity),
        createdAt: new Date().toISOString(),
    };
    user.orders.unshift(record);
    saveUsers(users);
}

function applyTradesToHistory(trades) {
    if (!Array.isArray(trades) || trades.length === 0) {
        return;
    }
    const executedAt = new Date().toISOString();
    for (const trade of trades) {
        const buyer = users.users.find((user) => Number(user.traderId) === Number(trade.buyTraderId));
        const seller = users.users.find((user) => Number(user.traderId) === Number(trade.sellTraderId));
        function recordTrade(user, side, orderId, counterpartyId) {
            if (!user) return;
            ensureUserHistory(user);
            const order = user.orders.find((item) => Number(item.id) === Number(orderId));
            if (order) {
                order.filledQuantity = Number(order.filledQuantity || 0) + Number(trade.quantity);
                order.remainingQuantity = Math.max(0, Number(order.quantity) - order.filledQuantity);
                order.status = order.remainingQuantity === 0 ? "FILLED" : "PARTIAL";
            }
            user.trades.unshift({
                orderId: Number(orderId),
                side,
                symbol: trade.symbol,
                quantity: Number(trade.quantity),
                price: Number(trade.price),
                executedAt,
                counterpartyTraderId: Number(counterpartyId),
                buyOrderId: Number(trade.buyOrderId),
                sellOrderId: Number(trade.sellOrderId),
            });
        }
        recordTrade(buyer, "BUY", trade.buyOrderId, trade.sellTraderId);
        recordTrade(seller, "SELL", trade.sellOrderId, trade.buyTraderId);
    }
    saveUsers(users);
}

for (const user of users.users) ensureUserHistory(user);
saveUsers(users);
const engine = spawn(ENGINE_PATH, [], { stdio: ["pipe", "pipe", "pipe"] });

let engineBuffer = "";
let pending = null;

engine.stdout.on("data", (chunk) => {
    engineBuffer += chunk.toString();
    const lines = engineBuffer.split(/\r?\n/);
    engineBuffer = lines.pop() || "";
    for (const line of lines) {
        if (!line.trim()) continue;
        if (line === "END") {
            const resolver = pending;
            pending = null;
            if (resolver) resolver(linesForRequest.splice(0));
        } else {
            linesForRequest.push(line);
        }
    }
});

engine.stderr.on("data", (chunk) => console.error("C++:", chunk.toString().trim()));

engine.on("error", (err) => {
    console.error(`Could not start the C++ engine at ${ENGINE_PATH}: ${err.message}`);
    console.error("Compile it first, e.g. from cpp/: g++ -std=c++17 models.cpp data_structures.cpp trading_engine.cpp engine_bridge.cpp -o engine_bridge");
    process.exit(1);
});

engine.on("close", (code) => {
    console.log(`C++ engine stopped: ${code}`);
    // Unblock any request still waiting on the engine instead of hanging forever.
    if (pending) {
        const resolver = pending;
        pending = null;
        resolver(["ERROR|C++ engine stopped unexpectedly"]);
    }
});
let linesForRequest = [];
let engineCommandQueue = Promise.resolve();

function sendEngineCommand(command) {
    const run = () =>
        new Promise((resolve, reject) => {
            if (!engine.stdin || !engine.stdin.writable) return reject(new Error("C++ engine unavailable"));
            if (pending) return reject(new Error("Internal engine queue error: command already pending"));
            pending = resolve;
            engine.stdin.write(command + "\n", (err) => {
                if (err) {
                    pending = null;
                    reject(err);
                }
            });
        });
    const result = engineCommandQueue.then(run, run);
    engineCommandQueue = result.catch(() => {});
    return result;
}

function auth(req, res, next) {
    const header = req.headers.authorization || "";
    const token = header.startsWith("Bearer ") ? header.slice(7) : "";
    const username = sessions.get(token);
    if (!username) return res.status(401).json({ success: false, message: "Login required" });
    const user = users.users.find((u) => u.username === username);
    if (!user) return res.status(401).json({ success: false, message: "Account not found" });
    req.user = user;
    next();
}

function findNumber(obj, paths) {
    for (const path of paths) {
        let value = obj;
        for (const key of path.split(".")) value = value?.[key];
        if (value !== undefined && value !== null && value !== "") {
            const n = Number(String(value).replace(/,/g, ""));
            if (Number.isFinite(n)) return n;
        }
    }
    return null;
}

const TICKER_ALIASES = {
    INFY: "Infosys", TCS: "Tata Consultancy Services", TATASTEEL: "Tata Steel",
    RELIANCE: "Reliance Industries", HDFCBANK: "HDFC Bank", ICICIBANK: "ICICI Bank",
    SBIN: "State Bank of India", BHARTIARTL: "Bharti Airtel", ITC: "ITC",
    LT: "Larsen & Toubro", HINDUNILVR: "Hindustan Unilever", AXISBANK: "Axis Bank",
    KOTAKBANK: "Kotak Mahindra Bank", BAJFINANCE: "Bajaj Finance", MARUTI: "Maruti Suzuki",
    SUNPHARMA: "Sun Pharmaceutical", TATAMOTORS: "Tata Motors", TITAN: "Titan Company",
    ASIANPAINT: "Asian Paints", NTPC: "NTPC", POWERGRID: "Power Grid Corporation",
    ONGC: "Oil and Natural Gas Corporation", WIPRO: "Wipro", TECHM: "Tech Mahindra",
    ULTRACEMCO: "UltraTech Cement", JSWSTEEL: "JSW Steel", ADANIPORTS: "Adani Ports",
    COALINDIA: "Coal India", NESTLEIND: "Nestle India", HCLTECH: "HCL Technologies",
    INDUSINDBK: "IndusInd Bank", DRREDDY: "Dr Reddys Laboratories", CIPLA: "Cipla",
    EICHERMOT: "Eicher Motors", GRASIM: "Grasim Industries", DIVISLAB: "Divis Laboratories",
    BPCL: "Bharat Petroleum", HEROMOTOCO: "Hero MotoCorp", APOLLOHOSP: "Apollo Hospitals",
    TRENT: "Trent", PCJEWELLER: "PC Jeweller", VEDPOWER: "Vedanta Power",
    IDEA: "Vodafone Idea", IDBI: "IDBI Bank", MONEYVIEW: "Moneyview",
    MEESHO: "Meesho", YESBANK: "Yes Bank", TATAPOWER: "Tata Power"
};

function providerStockName(query) {
    const cleaned = String(query || "").trim();
    return TICKER_ALIASES[cleaned.toUpperCase()] || cleaned;
}

async function indianApi(endpoint, options = {}) {
    if (!API_KEY) throw new Error("INDIAN_API_KEY is missing in backend/.env");
    const response = await fetch(API_BASE + endpoint, {
        ...options,
        headers: { accept: "application/json", "X-API-Key": API_KEY, ...(options.headers || {}) },
    });
    const data = await response.json();
    if (!response.ok) throw new Error(data?.message || `Indian API error: ${response.status}`);
    return data;
}

async function restoreUsersIntoEngine() {
    let changed = false;
    for (const user of users.users) {
        // The in-memory C++ order book is recreated on restart, so stale open orders cannot remain active.
        for (const order of user.orders || []) {
            if (order.status === "OPEN" || order.status === "PARTIAL") {
                order.status = "CANCELLED_RESTART";
                changed = true;
            }
        }
        if (!user.portfolioSeeded) {
            user.holdings = makeStarterPortfolio(user.holdings || []);
            user.portfolioSeeded = true;
            changed = true;
        }
        if (!Number.isFinite(Number(user.balance))) user.balance = STARTING_CASH;
        const restored = await sendEngineCommand(
            `RESTORE_TRADER|${user.traderId}|${String(user.name).replaceAll("|", " ")}|${Number(user.balance)}`,
        );
        const restoreError = restored.find((line) => line.startsWith("ERROR|"));
        if (restoreError) throw new Error(`Could not restore trader ${user.traderId}: ${restoreError.slice(6)}`);
        for (const holding of user.holdings || []) {
            const seeded = await sendEngineCommand(
                `SEED_HOLDING|${user.traderId}|${holding.symbol}|${holding.quantity}`,
            );
            const seedError = seeded.find((line) => line.startsWith("ERROR|"));
            if (seedError)
                throw new Error(
                    `Could not restore ${holding.symbol} for trader ${user.traderId}: ${seedError.slice(6)}`,
                );
        }
    }
    if (changed) saveUsers(users);
}

function parseAccount(lines) {
    const line = lines.find((x) => x.startsWith("ACCOUNT|"));
    if (!line) return null;
    const p = line.split("|");
    const count = Number(p[5]);
    const holdings = [];
    let i = 6;
    for (let n = 0; n < count; n++) {
        holdings.push({
            symbol: p[i],
            quantity: Number(p[i + 1]),
            reserved: Number(p[i + 2]),
            available: Number(p[i + 3]),
        });
        i += 4;
    }
    return {
        traderId: Number(p[1]),
        balance: Number(p[2]),
        reservedCash: Number(p[3]),
        availableBalance: Number(p[4]),
        holdings,
    };
}

async function syncUserAccount(user) {
    const lines = await sendEngineCommand(`ACCOUNT|${user.traderId}`);
    const account = parseAccount(lines);
    if (!account) return;
    user.balance = account.balance;
    user.holdings = account.holdings.map((h) => ({ symbol: h.symbol, quantity: h.quantity }));
    saveUsers(users);
    return account;
}

function parseTrades(lines) {
    return lines
        .filter((x) => x.startsWith("TRADE|"))
        .map((line) => {
            const p = line.split("|");
            return {
                buyOrderId: Number(p[1]),
                sellOrderId: Number(p[2]),
                buyTraderId: Number(p[3]),
                sellTraderId: Number(p[4]),
                symbol: p[5],
                price: Number(p[6]),
                quantity: Number(p[7]),
            };
        });
}

app.get("/api/health", (req, res) => res.json({ success: true, engine: !engine.killed, marketApi: Boolean(API_KEY) }));

app.post("/api/auth/signup", async (req, res) => {
    try {
        const name = String(req.body.name || "").trim();
        const username = String(req.body.username || "")
            .trim()
            .toLowerCase();
        const password = String(req.body.password || "");
        const initialBalance = STARTING_CASH;
        if (!name || !/^[a-z0-9\_.-]{3,24}$/.test(username) || password.length < 6)
            return res.status(400).json({ success: false, message: "Enter valid account details" });
        if (users.users.some((u) => u.username === username))
            return res.status(409).json({ success: false, message: "Username already exists" });
        const created = await sendEngineCommand(`CREATE_TRADER|${name.replaceAll("|", " ")}|${initialBalance}`);
        const line = created.find((x) => x.startsWith("TRADER_CREATED|"));
        if (!line) throw new Error("C++ could not create trader");
        const p = line.split("|");
        const credentials = hashPassword(password);
        const traderId = Number(p[1]);
        const holdings = makeStarterPortfolio();
        // Starter shares are free virtual holdings. The ₹1,00,000 cash balance is unchanged.
        for (const holding of holdings) {
            const seeded = await sendEngineCommand(`SEED_HOLDING|${traderId}|${holding.symbol}|${holding.quantity}`);
            const seedError = seeded.find((line) => line.startsWith("ERROR|"));
            if (seedError) throw new Error(`Starter portfolio setup failed: ${seedError.slice(6)}`);
        }
        const user = {
            username,
            name,
            salt: credentials.salt,
            passwordHash: credentials.hash,
            traderId,
            balance: initialBalance,
            holdings,
            portfolioSeeded: true,
            orders: [],
            trades: [],
            allocations: [],
        };
        users.users.push(user);
        saveUsers(users);
        const token = issueToken(username);
        res.json({ success: true, token, user: { username, name, traderId: user.traderId } });
    } catch (error) {
        res.status(500).json({ success: false, message: error.message });
    }
});

app.post("/api/auth/login", (req, res) => {
    try {
        const username = String(req.body.username || "")
            .trim()
            .toLowerCase();
        const password = String(req.body.password || "");
        const user = users.users.find((u) => u.username === username);
        if (!user || !verifyPassword(password, user.salt, user.passwordHash))
            return res.status(401).json({ success: false, message: "Invalid username or password" });
        const token = issueToken(username);
        res.json({ success: true, token, user: { username, name: user.name, traderId: user.traderId } });
    } catch (error) {
        res.status(500).json({ success: false, message: "Login failed" });
    }
});

app.post("/api/auth/logout", auth, (req, res) => {
    const token = (req.headers.authorization || "").slice(7);
    sessions.delete(token);
    res.json({ success: true });
});

app.get("/api/auth/me", auth, async (req, res) => {
    const account = await syncUserAccount(req.user);
    res.json({
        success: true,
        user: { username: req.user.username, name: req.user.name, traderId: req.user.traderId },
        account,
    });
});

app.get("/api/stock/search", async (req, res) => {
    try {
        const query = String(req.query.q || "").trim();
        if (!query) return res.json({ success: true, data: [] });
        const data = await indianApi(`/stock?name=${encodeURIComponent(providerStockName(query))}`);
        if (!data || data.error || data.message === "Stock not found") {
            return res.status(404).json({ success: false, message: `Stock not found: ${query}`, data: [] });
        }
        const price = findNumber(data, [
            "stockDetailsReusableData.currentPrice.NSE",
            "stockDetailsReusableData.currentPrice",
            "stockDetailsReusableData.nsePrice",
            "currentPrice.NSE",
            "currentPrice",
            "price",
        ]);
        const symbol = data.exchangeCodeNse || data.ticker || data.symbol || data.tickerId || query.toUpperCase();
        const name = data.companyName || data.company || data.name || providerStockName(query);
        return res.json({ success: true, data: [{ symbol, name, price, raw: data }] });
    } catch (error) {
        const notFound = /stock not found/i.test(error.message || "");
        return res
            .status(notFound ? 404 : 502)
            .json({
                success: false,
                message: notFound ? `Stock not found: ${String(req.query.q || "")}` : error.message,
                data: [],
            });
    }
});

app.get("/api/stock/active", async (req, res) => {
    try {
        const data = await indianApi("/NSE_most_active");
        res.json({ success: true, data });
    } catch (error) {
        res.status(502).json({ success: false, message: error.message });
    }
});

// Agentic market research endpoint: scan available market data, score candidates,
// then optionally ask an LLM to explain the evidence. It never places orders.
app.get("/api/ai/market-brief", auth, async (req, res) => {
    try {
        const raw = await indianApi("/NSE_most_active");
        const stocks = Array.isArray(raw) ? raw : raw?.data || raw?.stocks || raw?.trending_stocks || [];
        if (!Array.isArray(stocks) || stocks.length === 0) {
            return res.status(502).json({ success: false, message: "The market provider returned no active stocks." });
        }
        const candidates = stocks
            .map((item) => {
                const symbol = String(item.ticker || item.tickerId || item.symbol || "").toUpperCase();
                const name = String(item.company || item.companyName || item.name || symbol);
                const price = Number(item.price ?? item.currentPrice ?? item.close);
                const change = Number(
                    item.percent_change ?? item.percentChange ?? item.netChangePercent ?? item.changePercent,
                );
                const netChange = Number(item.net_change ?? item.netChange ?? 0);
                const volume = Number(item.volume ?? item.tradedVolume ?? 0);
                const validChange = Number.isFinite(change) ? change : 0;
                const validVolume = Number.isFinite(volume) && volume > 0 ? volume : 0;
                return {
                    symbol,
                    name,
                    price: Number.isFinite(price) && price > 0 ? price : null,
                    percentChange: validChange,
                    netChange: Number.isFinite(netChange) ? netChange : 0,
                    volume: validVolume,
                    high: Number(item.high) || null,
                    low: Number(item.low) || null,
                    providerRating: item.overall_rating || null,
                };
            })
            .filter((x) => x.symbol && x.price !== null);
        const maxVolume = Math.max(1, ...candidates.map((x) => x.volume));
        const ranked = candidates
            .map((stock) => {
                const volumeScore = stock.volume ? (stock.volume / maxVolume) * 25 : 0;
                const momentumScore = Math.min(50, Math.abs(stock.percentChange) * 5);
                const score = Math.round(
                    Math.min(100, momentumScore + volumeScore + (stock.percentChange > 0 ? 25 : 0)),
                );
                const direction =
                    stock.percentChange > 0.25 ? "positive" : stock.percentChange < -0.25 ? "negative" : "mixed";
                const reasons = [];
                if (Math.abs(stock.percentChange) >= 2)
                    reasons.push(
                        `large reported move (${stock.percentChange > 0 ? "+" : ""}${stock.percentChange.toFixed(2)}%)`,
                    );
                else if (Math.abs(stock.percentChange) >= 0.5)
                    reasons.push(
                        `noticeable reported move (${stock.percentChange > 0 ? "+" : ""}${stock.percentChange.toFixed(2)}%)`,
                    );
                if (stock.volume) reasons.push("included in the provider's most-active list");
                if (!reasons.length) reasons.push("limited momentum evidence in the available snapshot");
                return { ...stock, score, direction, reasons };
            })
            .sort((a, b) => b.score - a.score)
            .slice(0, 6);
        let summary =
            "Ranked using reported percentage move and relative volume from the provider's most-active snapshot. This is a screening signal, not a prediction.";
        let aiMode = "rules-based explanation (no AI key configured)";
        if (process.env.OPENAI_API_KEY && ranked.length) {
            try {
                const response = await fetch("https://api.openai.com/v1/chat/completions", {
                    method: "POST",
                    headers: {
                        "Content-Type": "application/json",
                        Authorization: `Bearer ${process.env.OPENAI_API_KEY}`,
                    },
                    body: JSON.stringify({
                        model: process.env.OPENAI_MODEL || "gpt-4o-mini",
                        temperature: 0.2,
                        messages: [
                            {
                                role: "system",
                                content:
                                    "You are MarketSim's cautious market research assistant for a virtual trading simulator. Use only supplied data. Do not invent news, causes, fundamentals, or prices. Do not promise returns or give personalized financial advice. Explain what the snapshot shows, note uncertainty and data limitations, and remind the user this is not a prediction. Keep the response under 120 words.",
                            },
                            {
                                role: "user",
                                content: `Analyze this current provider snapshot. Explain the strongest observable signals and limitations; do not claim to have checked news. Data: ${JSON.stringify(ranked.map(({ symbol, name, price, percentChange, volume, score, direction, reasons }) => ({ symbol, name, price, percentChange, volume, score, direction, reasons })))}`,
                            },
                        ],
                    }),
                });
                const body = await response.json();
                if (!response.ok) throw new Error(body?.error?.message || `AI provider error ${response.status}`);
                summary = body.choices?.[0]?.message?.content?.trim() || summary;
                aiMode = "AI-assisted explanation grounded in provider snapshot";
            } catch (aiError) {
                console.warn("MarketSim AI explanation unavailable:", aiError.message);
            }
        }
        return res.json({
            success: true,
            data: {
                asOf: new Date().toISOString(),
                source: "Indian stock data provider · NSE most-active snapshot",
                aiMode,
                summary,
                limitation:
                    "Snapshot data may be delayed or incomplete. News research is not connected yet; no trade is placed by this assistant.",
                candidates: ranked,
            },
        });
    } catch (error) {
        console.error("Market brief failed:", error.message);
        return res.status(502).json({ success: false, message: error.message || "Market scan failed." });
    }
});

app.get("/api/stock/history/:symbol", async (req, res) => {
    try {
        const period = ["1m", "6m", "1yr", "3yr", "5yr"].includes(req.query.period) ? req.query.period : "1m";
        const data = await indianApi(
            `/historical_data?stock_name=${encodeURIComponent(req.params.symbol)}&period=${period}&filter=price`,
        );
        const dataset = (data.datasets || []).find((x) => String(x.metric).toLowerCase() === "price");
        const points = (dataset?.values || [])
            .map((x) => ({ date: x[0], price: Number(x[1]) }))
            .filter((x) => Number.isFinite(x.price));
        res.json({ success: true, data: points });
    } catch (error) {
        res.status(502).json({ success: false, message: error.message });
    }
});

app.get("/api/quote/:symbol", async (req, res) => {
    try {
        const data = await indianApi(`/stock?name=${encodeURIComponent(providerStockName(req.params.symbol))}`);
        if (!data || data.error || data.message === "Stock not found") {
            return res.status(404).json({ success: false, message: `Stock not found: ${req.params.symbol}` });
        }
        const price = findNumber(data, [
            "stockDetailsReusableData.currentPrice.NSE",
            "stockDetailsReusableData.currentPrice",
            "stockDetailsReusableData.nsePrice",
            "currentPrice.NSE",
            "currentPrice",
            "price",
        ]);
        const percentChange = findNumber(data, [
            "stockDetailsReusableData.percentChange",
            "percentChange",
            "stockTechnicalData.percentChange",
        ]);
        res.json({
            success: true,
            data: {
                symbol:
                    data.exchangeCodeNse ||
                    data.ticker ||
                    data.symbol ||
                    data.tickerId ||
                    req.params.symbol.toUpperCase(),
                name: data.companyName || data.company || req.params.symbol.toUpperCase(),
                price,
                percentChange,
            },
        });
    } catch (error) {
        res.status(502).json({ success: false, message: error.message });
    }
});

app.get("/api/account", auth, async (req, res) => {
    try {
        res.json({ success: true, data: await syncUserAccount(req.user) });
    } catch (error) {
        res.status(500).json({ success: false, message: error.message });
    }
});

// Allocate starting virtual holdings using verified market price.
app.post("/api/portfolio/allocate", auth, async (req, res) => {
    try {
        const requestedSymbol = String(req.body.symbol || "")
            .trim()
            .toUpperCase();
        const quantity = Number(req.body.quantity);
        if (!/^[A-Z0-9&.\_-]{1,25}$/.test(requestedSymbol)) {
            return res.status(400).json({
                success: false,
                message: "Select a valid stock first",
            });
        }
        if (!Number.isSafeInteger(quantity) || quantity <= 0 || quantity > 1000000) {
            return res.status(400).json({
                success: false,
                message: "Quantity must be a positive whole number",
            });
        }
        // Fetch the price on the backend; never trust a browser price.
        const quote = await indianApi(`/stock?name=${encodeURIComponent(requestedSymbol)}`);
        const price = findNumber(quote, [
            "stockDetailsReusableData.currentPrice.NSE",
            "stockDetailsReusableData.currentPrice",
            "stockDetailsReusableData.nsePrice",
            "currentPrice.NSE",
            "currentPrice",
            "price",
        ]);
        const symbol = String(quote.tickerId || requestedSymbol)
            .trim()
            .toUpperCase();
        if (!Number.isFinite(price) || price <= 0) {
            return res.status(502).json({
                success: false,
                message: "Could not verify the stock price",
            });
        }
        if (symbol !== requestedSymbol) {
            return res.status(400).json({
                success: false,
                message: `Verified symbol is ${symbol}. Search and select it again.`,
            });
        }
        const lines = await sendEngineCommand(`ALLOCATE_HOLDING|${req.user.traderId}|${symbol}|${price}|${quantity}`);
        const errorLine = lines.find((line) => line.startsWith("ERROR|"));
        if (errorLine) {
            return res.status(400).json({
                success: false,
                message: errorLine.slice(6),
            });
        }
        const allocationLine = lines.find((line) => line.startsWith("ALLOCATION_OK|"));
        if (!allocationLine) {
            return res.status(500).json({
                success: false,
                message: "C++ did not confirm the portfolio allocation",
            });
        }
        const parts = allocationLine.split("|");
        const account = parseAccount(lines);
        if (!account) {
            return res.status(500).json({
                success: false,
                message:
                    "Allocation result received, but account details are unavailable. Refresh the account before retrying.",
            });
        }
        req.user.balance = account.balance;
        req.user.holdings = account.holdings.map((holding) => ({
            symbol: holding.symbol,
            quantity: holding.quantity,
        }));
        if (!Array.isArray(req.user.allocations)) {
            req.user.allocations = [];
        }
        const allocation = {
            symbol: parts[2],
            price: Number(parts[3]),
            quantity: Number(parts[4]),
            totalCost: Number(parts[5]),
            createdAt: new Date().toISOString(),
            type: "INITIAL_PORTFOLIO_ALLOCATION",
        };
        req.user.allocations.unshift(allocation);
        saveUsers(users);
        res.json({
            success: true,
            message: `Allocated ${quantity} shares of ${symbol}. ₹${allocation.totalCost.toLocaleString("en-IN")} deducted from virtual cash. This is portfolio setup, not a matched trade.`,
            allocation,
            account,
        });
    } catch (error) {
        res.status(500).json({
            success: false,
            message: error.message,
        });
    }
});

app.get("/api/history", auth, (req, res) => {
    try {
        ensureUserHistory(req.user);
        res.json({
            success: true,
            data: {
                orders: req.user.orders.slice(0, 50),
                trades: req.user.trades.slice(0, 50),
            },
        });
    } catch (error) {
        res.status(500).json({ success: false, message: error.message });
    }
});

app.post("/api/order/:id/cancel", auth, async (req, res) => {
    try {
        const orderId = Number(req.params.id);
        if (!Number.isSafeInteger(orderId) || orderId <= 0) {
            return res.status(400).json({ success: false, message: "Invalid order ID" });
        }

        ensureUserHistory(req.user);
        const historyOrder = req.user.orders.find((item) => Number(item.id) === orderId);
        if (!historyOrder) {
            return res.status(404).json({ success: false, message: "Order not found in your account" });
        }
        if (historyOrder.status !== "OPEN" && historyOrder.status !== "PARTIAL") {
            return res.status(409).json({
                success: false,
                message: `This order cannot be cancelled because its status is ${historyOrder.status || "unknown"}`,
            });
        }

        // The C++ engine verifies ownership again and releases reservations for
        // the remaining quantity only. It is the source of truth for open orders.
        const lines = await sendEngineCommand(`CANCEL|${req.user.traderId}|${orderId}`);
        const errorLine = lines.find((line) => line.startsWith("ERROR|"));
        if (errorLine) {
            return res.status(409).json({ success: false, message: errorLine.slice(6) });
        }
        const cancelledLine = lines.find((line) => line.startsWith("CANCELLED|"));
        if (!cancelledLine) {
            return res.status(500).json({ success: false, message: "Matching engine did not confirm cancellation" });
        }

        const parts = cancelledLine.split("|");
        historyOrder.status = "CANCELLED";
        historyOrder.remainingQuantity = 0;
        historyOrder.cancelledQuantity = Number(parts[6] || 0);
        historyOrder.cancelledAt = new Date().toISOString();
        saveUsers(users);

        const account = await syncUserAccount(req.user);
        res.json({
            success: true,
            message: `Order #${orderId} cancelled successfully`,
            order: historyOrder,
            account,
        });
    } catch (error) {
        res.status(500).json({ success: false, message: error.message });
    }
});

app.get("/api/orderbook/:symbol", auth, async (req, res) => {
    try {
        const lines = await sendEngineCommand(`STATUS|${req.params.symbol.toUpperCase()}`);
        let side = null;
        const levels = { BUY: [], SELL: [] };
        let current = null;
        for (const line of lines) {
            if (line.startsWith("SIDE|")) {
                // Save the previous side's last price level first.
                if (current && side) {
                    levels[side].push(current);
                }
                current = null;
                const nextSide = line.split("|")[1];
                side = nextSide === "BUY" || nextSide === "SELL" ? nextSide : null;
                continue;
            }
            if (line.startsWith("LEVEL|")) {
                if (current && side) levels[side].push(current);
                const p = line.split("|");
                current = { price: Number(p[1]), orderCount: Number(p[2]), quantity: 0, orders: [] };
                continue;
            }
            if (line.startsWith("ORDER|")) {
                const p = line.split("|");
                const order = { id: Number(p[1]), traderId: Number(p[2]), quantity: Number(p[3]), price: Number(p[4]) };
                if (current) {
                    current.orders.push(order);
                    current.quantity += order.quantity;
                }
            }
        }
        if (current && side) levels[side].push(current);
        res.json({ success: true, data: levels });
    } catch (error) {
        res.status(500).json({ success: false, message: error.message });
    }
});

app.post("/api/order", auth, async (req, res) => {
    try {
        const requestedType = String(req.body.type || "")
            .trim()
            .toUpperCase();
        const type = requestedType === "BUY" ? "B" : requestedType === "SELL" ? "S" : "";
        const symbol = String(req.body.symbol || "")
            .trim()
            .toUpperCase();
        const rawPrice = Number(req.body.price);
        const price = Math.round(rawPrice * 100) / 100; // engine works in 2 decimals (paise)
        const quantity = Number(req.body.quantity);
        if (!type || !symbol || !Number.isFinite(price) || !Number.isInteger(quantity) || price < 0.01 || quantity <= 0)
            return res.status(400).json({ success: false, message: "Invalid order" });
        const lines = await sendEngineCommand(`ADD|${req.user.traderId}|${type}|${symbol}|${price}|${quantity}`);
        const errorLine = lines.find((x) => x.startsWith("ERROR|"));
        if (errorLine) return res.status(400).json({ success: false, message: errorLine.slice(6) });
        const trades = parseTrades(lines);
        const orderLine = lines.find((x) => x.startsWith("ORDER_ADDED|"));
        if (!orderLine) return res.status(500).json({ success: false, message: "C++ engine did not return an order" });
        const p = orderLine.split("|");
        const order = {
            id: Number(p[1]),
            traderId: Number(p[2]),
            type: p[3] === "B" ? "BUY" : "SELL",
            symbol: p[4],
            price: Number(p[5]),
            quantity: Number(p[6]),
        };
        addOrderToHistory(req.user, order);
        applyTradesToHistory(trades);
        const affectedIds = new Set([Number(req.user.traderId)]);
        for (const trade of trades) {
            affectedIds.add(Number(trade.buyTraderId));
            affectedIds.add(Number(trade.sellTraderId));
        }
        for (const traderId of affectedIds) {
            const affectedUser = users.users.find((u) => Number(u.traderId) === traderId);
            if (affectedUser) await syncUserAccount(affectedUser);
        }
        res.json({ success: true, order, trades, historyUpdated: true });
    } catch (error) {
        res.status(500).json({ success: false, message: error.message });
    }
});

process.once("SIGINT", () => {
    try {
        engine.stdin.write("EXIT\n");
    } catch {}
    process.exit(0);
});

restoreUsersIntoEngine()
    .then(() => {
        app.listen(PORT, () => console.log(`Web app running at http://localhost:${PORT}`));
    })
    .catch((error) => {
        console.error("Startup failed:", error);
        process.exit(1);
    });