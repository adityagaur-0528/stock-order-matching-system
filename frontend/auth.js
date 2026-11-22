const tabs = document.querySelectorAll(".tab");
const loginForm = document.getElementById("loginForm");
const signupForm = document.getElementById("signupForm");
const message = document.getElementById("authMessage");

if (localStorage.getItem("marketsim_token")) window.location.replace("/");

tabs.forEach(tab => tab.addEventListener("click", () => {
    tabs.forEach(item => item.classList.remove("active"));
    tab.classList.add("active");
    const signup = tab.dataset.tab === "signup";
    loginForm.classList.toggle("hidden", signup);
    signupForm.classList.toggle("hidden", !signup);
    message.textContent = "";
}));

async function submitAuth(url, body) {
    const response = await fetch(url, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(body)
    });
    const data = await response.json().catch(() => ({}));
    if (!response.ok || !data.token) throw new Error(data.message || "Authentication failed");
    localStorage.setItem("marketsim_token", data.token);
    window.location.replace("/");
}

loginForm.addEventListener("submit", async event => {
    event.preventDefault();
    message.textContent = "Signing in…";
    try {
        await submitAuth("/api/auth/login", {
            username: document.getElementById("loginUsername").value,
            password: document.getElementById("loginPassword").value
        });
    } catch (error) { message.textContent = error.message; }
});

signupForm.addEventListener("submit", async event => {
    event.preventDefault();
    message.textContent = "Creating your account and starter portfolio…";
    try {
        await submitAuth("/api/auth/signup", {
            name: document.getElementById("signupName").value,
            username: document.getElementById("signupUsername").value,
            password: document.getElementById("signupPassword").value,
            initialBalance: 100000
        });
    } catch (error) { message.textContent = error.message; }
});
