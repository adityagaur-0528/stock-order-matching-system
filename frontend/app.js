(() => {
    const token = localStorage.getItem("marketsim_token");
    if (!token) { window.location.replace("/login.html"); return; }

    const $ = id => document.getElementById(id);
    const state = { symbol: "INFY", name: "Infosys", side: "BUY", chart: null, account: null };

    async function api(url, options = {}) {
        const response = await fetch(url, {
            ...options,
            headers: {
                ...(options.headers || {}),
                ...(options.body ? { "Content-Type": "application/json" } : {}),
                Authorization: `Bearer ${token}`
            }
        });
        const data = await response.json().catch(() => ({}));
        if (response.status === 401) {
            localStorage.removeItem("marketsim_token");
            window.location.replace("/login.html");
            throw new Error("Please log in again.");
        }
        if (!response.ok || data.success === false) throw new Error(data.message || `Request failed (${response.status})`);
        return data;
    }

    const money = value => Number(value || 0).toLocaleString("en-IN", { style: "currency", currency: "INR", maximumFractionDigits: 2 });
    const safe = value => String(value ?? "").replace(/[&<>"']/g, ch => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[ch]));
    const setMessage = (text, ok = false) => { $("orderMessage").textContent = text; $("orderMessage").className = `message ${ok ? "success" : ""}`; };

    async function loadAccount() {
        const result = await api("/api/account");
        const account = result.data;
        state.account = account;
        $("cashAvailable").textContent = money(account.availableBalance);
        $("holdingCount").textContent = account.holdings.length;
        $("holdingsList").innerHTML = account.holdings.length ? account.holdings.map(h =>
            `<div class="holding-row"><span>${safe(h.symbol)}</span><strong>${h.quantity}</strong><small>${h.available} available${h.reserved ? ` · ${h.reserved} reserved` : ""}</small></div>`
        ).join("") : '<div class="empty">No holdings yet.</div>';
    }

    async function loadHistory() {
        const result = await api("/api/history");
        const orders = result.data.orders || [];
        const trades = result.data.trades || [];
        $("openOrderCount").textContent = orders.filter(o => o.status === "OPEN" || o.status === "PARTIAL").length;
        $("tradeCount").textContent = trades.length;
        $("ordersTableBody").innerHTML = orders.length ? orders.map(o => {
            const status = o.status || "OPEN";
            const filled = Number(o.filledQuantity || 0);
            const remaining = Number(o.remainingQuantity ?? Math.max(0, Number(o.quantity) - filled));
            const cancelled = Number(o.cancelledQuantity ?? (status === "CANCELLED" ? Math.max(0, Number(o.quantity) - filled) : 0));
            const canCancel = status === "OPEN" || status === "PARTIAL";
            // Make the terminal state understandable: filled quantity stays visible,
            // and the quantity removed from the book is explicitly labelled cancelled.
            const executionSummary = status === "CANCELLED"
                ? (filled > 0
                    ? `${filled} filled · ${cancelled} cancelled`
                    : `0 filled · ${cancelled} cancelled`)
                : status === "CANCELLED_RESTART"
                    ? `${filled} filled · ${remaining} remaining at restart`
                    : `${filled} filled · ${remaining} remaining`;
            const displayStatus = status === "CANCELLED"
                ? (filled > 0 ? "CANCELLED · PARTIALLY FILLED" : "CANCELLED")
                : status;
            return `<tr><td class="${o.type === "BUY" ? "buy-text" : "sell-text"}">${safe(o.type)}</td><td>${safe(o.symbol)}</td><td>${o.quantity}</td><td>${money(o.price)}</td><td>${money(o.price * o.quantity)}</td><td>${executionSummary}</td><td>${safe(displayStatus)}</td><td>${canCancel ? `<button type="button" class="cancel-order-btn" data-cancel-order="${Number(o.id)}">Cancel</button>` : '<span class="muted">—</span>'}</td></tr>`;
        }).join("") : '<tr><td colspan="8" class="empty">No orders yet.</td></tr>';
        $("tradesTableBody").innerHTML = trades.length ? trades.map(t =>
            `<tr><td class="${t.side === "BUY" ? "buy-text" : "sell-text"}">${safe(t.side)}</td><td>${safe(t.symbol)}</td><td>${t.quantity}</td><td>${money(t.price)}</td><td>${money(t.price * t.quantity)}</td><td>${safe(new Date(t.executedAt).toLocaleString())}</td></tr>`
        ).join("") : '<tr><td colspan="6" class="empty">No completed trades yet.</td></tr>';
        $("tradeFeed").innerHTML = trades.slice(0, 8).length ? trades.slice(0, 8).map(t =>
            `<div class="feed-row"><span class="${t.side === "BUY" ? "buy-text" : "sell-text"}">${safe(t.side)}</span><strong>${safe(t.symbol)}</strong><span>${t.quantity} @ ${money(t.price)}</span></div>`
        ).join("") : '<div class="empty">No trades yet.</div>';
    }

    async function cancelOrder(orderId, button) {
        if (!Number.isSafeInteger(orderId) || orderId <= 0) return;
        if (!window.confirm(`Cancel order #${orderId}? Any already executed quantity will remain unchanged.`)) return;
        button.disabled = true;
        button.textContent = "Cancelling…";
        try {
            const result = await api(`/api/order/${orderId}/cancel`, { method: "POST" });
            setMessage(result.message || `Order #${orderId} cancelled.`, true);
            await Promise.all([loadAccount(), loadHistory(), loadOrderBook()]);
        } catch (error) {
            setMessage(error.message);
            button.disabled = false;
            button.textContent = "Cancel";
        }
    }


async function loadOrderBook(symbol = state.symbol) {
    try {
        const result = await api(
            `/api/orderbook/${encodeURIComponent(symbol)}`
        );

        const buy = result.data.BUY || [];
        const sell = result.data.SELL || [];

    
        const allOrders = [
            ...buy.flatMap(level =>
                level.orders.map(order => ({
                    ...order,
                    side: "BUY"
                }))
            ),
            ...sell.flatMap(level =>
                level.orders.map(order => ({
                    ...order,
                    side: "SELL"
                }))
            )
        ];

        if (allOrders.length === 0) {
            $("depthRows").innerHTML =
                '<div class="empty">No open orders.</div>';

            $("bookStatus").textContent = "● Order book";
            return;
        }

        $("depthRows").innerHTML = allOrders.map(order => {
            const time = order.createdAt
                ? new Date(order.createdAt).toLocaleString("en-IN")
                : "Time unavailable";

            return `
                <div class="order-depth-row">
                    <div class="${order.side === "BUY" ? "buy-text" : "sell-text"}">
                        <strong>${order.side}</strong>
                    </div>

                    <div>#${Number(order.id)}</div>

                    <div>
                        ${safe(order.traderName)}
                        <small class="trader-id">
                            #${Number(order.traderId)}
                        </small>
                    </div>

                    <div>${Number(order.quantity)}</div>

                    <div>${money(order.price)}</div>

                    <div class="order-time">${safe(time)}</div>
                </div>
            `;
        }).join("");

        $("bookStatus").textContent = "● Order book";

    } catch (error) {
        $("depthRows").innerHTML =
            `<div class="empty">${safe(error.message)}</div>`;
    }
}


    async function loadChart(symbol = state.symbol) {
        try {
            const period = $("chartPeriod").value || "1m";
            const result = await api(`/api/stock/history/${encodeURIComponent(symbol)}?period=${encodeURIComponent(period)}`);
            const points = result.data || [];
            const canvas = $("priceChart");
            if (!window.Chart || !canvas) return;
            if (state.chart) state.chart.destroy();
            state.chart = new Chart(canvas, {
                type: "line",
                data: { labels: points.map(p => p.date), datasets: [{ label: `${symbol} price`, data: points.map(p => p.price), borderWidth: 2, pointRadius: 0, tension: 0.15 }] },
                options: { responsive: true, maintainAspectRatio: false, plugins: { legend: { display: false } }, scales: { x: { ticks: { maxTicksLimit: 6 } } } }
            });
        } catch (_) { /* chart history can be unavailable independently of order matching */ }
    }

    async function selectStock(symbol, fallbackName = symbol) {
        state.symbol = String(symbol || "").toUpperCase();
        state.name = fallbackName || state.symbol;
        if (!state.symbol) return;
        $("stockName").textContent = state.name;
        $("stockSymbol").textContent = state.symbol;
        $("stockPrice").textContent = "Loading…";
        try {
            const result = await api(`/api/quote/${encodeURIComponent(state.symbol)}`);
            const quote = result.data || {};
            state.name = quote.name || state.name;
            $("stockName").textContent = state.name;
            $("stockSymbol").textContent = quote.symbol || state.symbol;
            $("stockPrice").textContent = Number.isFinite(Number(quote.price)) ? money(quote.price) : "Price unavailable";
            const change = Number(quote.percentChange);
            $("stockChange").textContent = Number.isFinite(change) ? `${change > 0 ? "+" : ""}${change.toFixed(2)}%` : "Latest available quote";
            if (Number.isFinite(Number(quote.price)) && Number(quote.price) > 0) $("orderPrice").value = Number(quote.price).toFixed(2);
        } catch (error) {
            $("stockPrice").textContent = "Quote unavailable";
            $("stockChange").textContent = error.message;
        }
        await Promise.all([loadOrderBook(state.symbol), loadChart(state.symbol)]);
    }

    async function searchStock() {
        const query = $("stockSearch").value.trim();
        if (!query) return;
        try {
            const result = await api(`/api/stock/search?q=${encodeURIComponent(query)}`);
            const item = (result.data || [])[0];
            if (!item || !item.symbol) throw new Error("No stock found. Try its ticker symbol.");
            await selectStock(item.symbol, item.name || item.symbol);
        } catch (error) { $("stockName").textContent = error.message; }
    }

    async function loadActiveStocks() {
        try {
            const result = await api("/api/stock/active");
            let items = Array.isArray(result.data) ? result.data : (result.data?.data || result.data?.stocks || result.data?.trending_stocks || []);
            if (!Array.isArray(items)) items = [];
            items = items.slice(0, 12);
            $("activeStocks").innerHTML = items.length ? items.map(item => {
                const symbol = item.tickerId || item.symbol || item.ticker || item.companySymbol || "";
                const name = item.companyName || item.name || symbol;
                const price = item.currentPrice || item.price || item.close || item.lastPrice;
                if (!symbol) return "";
                return `<button class="active-stock" data-symbol="${safe(symbol)}" data-name="${safe(name)}"><span><strong>${safe(symbol)}</strong><small>${safe(name)}</small></span><b>${Number.isFinite(Number(price)) ? money(price) : "—"}</b></button>`;
            }).join("") : '<div class="empty">Market list unavailable.</div>';
            $("activeStocks").querySelectorAll("[data-symbol]").forEach(button => button.addEventListener("click", () => selectStock(button.dataset.symbol, button.dataset.name)));
        } catch (_) { $("activeStocks").innerHTML = '<div class="empty">Could not load market list.</div>'; }
    }

    async function runAiScan() {
        const button = $("runAiScan");
        const status = $("aiStatus");
        button.disabled = true;
        status.textContent = "Agent workflow: scanning market data and ranking candidates…";
        status.className = "message";
        $("aiSummary").textContent = "Analyzing provider snapshot…";
        $("aiCandidates").innerHTML = "";
        try {
            const result = await api("/api/ai/market-brief");
            const brief = result.data || {};
            $("aiSummary").textContent = brief.summary || "No explanation returned.";
            $("aiDisclaimer").textContent = `${brief.source || "Market data provider"} · ${brief.aiMode || "Market scan"} · ${brief.limitation || "Data can be delayed."} Updated ${brief.asOf ? new Date(brief.asOf).toLocaleString() : "just now"}.`;
            const candidates = Array.isArray(brief.candidates) ? brief.candidates : [];
            $("aiCandidates").innerHTML = candidates.length ? candidates.map((item, index) => {
                const change = Number(item.percentChange);
                const changeClass = change > 0 ? "buy-text" : change < 0 ? "sell-text" : "";
                return `<button type="button" class="ai-candidate" data-symbol="${safe(item.symbol)}" data-name="${safe(item.name)}"><span class="ai-rank">${index + 1}</span><span class="ai-stock"><strong>${safe(item.symbol)} · ${safe(item.name)}</strong><small>${safe((item.reasons || []).join("; "))}</small></span><span class="ai-metrics"><strong class="${changeClass}">${change > 0 ? "+" : ""}${change.toFixed(2)}%</strong><small>Signal score ${Number(item.score)}/100</small></span></button>`;
            }).join("") : '<div class="empty">No candidates could be ranked from the available data.</div>';
            $("aiCandidates").querySelectorAll("[data-symbol]").forEach(button => button.addEventListener("click", () => selectStock(button.dataset.symbol, button.dataset.name)));
            status.textContent = `Scan complete · ${candidates.length} candidates · ${brief.aiMode || "market scan"}`;
            status.className = "message success";
        } catch (error) {
            status.textContent = `Market scan failed: ${error.message}`;
            $("aiSummary").textContent = "Check that the market API is configured and available, then try again.";
        } finally { button.disabled = false; }
    }

    async function placeOrder() {
        const price = Number($("orderPrice").value);
        const quantity = Number($("orderQuantity").value);
        if (!state.symbol) return setMessage("Search and select a stock first.");
        if (!Number.isFinite(price) || price <= 0 || !Number.isSafeInteger(quantity) || quantity <= 0) return setMessage("Enter a valid price and whole-number quantity.");
        $("placeOrder").disabled = true;
        setMessage("Submitting order to matching engine…", true);
        try {
            const result = await api("/api/order", { method: "POST", body: JSON.stringify({ type: state.side, symbol: state.symbol, price, quantity }) });
            const trades = result.trades || [];
            setMessage(trades.length ? `Order accepted. ${trades.length} trade execution(s) completed.` : `Order #${result.order.id} accepted and added to the shared order book.`, true);
            await Promise.all([loadAccount(), loadHistory(), loadOrderBook(state.symbol)]);
        } catch (error) { setMessage(error.message); }
        finally { $("placeOrder").disabled = false; }
    }

    async function init() {
        try {
            const result = await api("/api/auth/me");
            $("userName").textContent = result.user.name;
            $("userId").textContent = `Trader #${result.user.traderId}`;
            await Promise.all([loadAccount(), loadHistory(), loadActiveStocks()]);
            await selectStock("INFY", "Infosys");
        } catch (error) { console.error(error); }
    }

    $("searchBtn").addEventListener("click", searchStock);
    $("runAiScan").addEventListener("click", runAiScan);
    $("stockSearch").addEventListener("keydown", event => { if (event.key === "Enter") searchStock(); });
    $("chartPeriod").addEventListener("change", () => loadChart());
    $("buyToggle").addEventListener("click", () => { state.side = "BUY"; $("buyToggle").classList.add("active"); $("sellToggle").classList.remove("active"); });
    $("sellToggle").addEventListener("click", () => { state.side = "SELL"; $("sellToggle").classList.add("active"); $("buyToggle").classList.remove("active"); });
    $("placeOrder").addEventListener("click", placeOrder);
    $("ordersTableBody").addEventListener("click", event => {
        const button = event.target.closest("[data-cancel-order]");
        if (!button) return;
        cancelOrder(Number(button.dataset.cancelOrder), button);
    });
    $("logoutBtn").addEventListener("click", async () => {
        try { await api("/api/auth/logout", { method: "POST" }); } catch (_) {}
        localStorage.removeItem("marketsim_token");
        window.location.replace("/login.html");
    });

    init();
    window.setInterval(() => { loadAccount().catch(() => {}); loadHistory().catch(() => {}); loadOrderBook().catch(() => {}); }, 10000);
})();
