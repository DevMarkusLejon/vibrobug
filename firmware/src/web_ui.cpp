#include "web_ui.h"

const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
  <title>VibroBug</title>
  <link rel="stylesheet" href="/style.css">
</head>
<body>
  <main>
    <header>
      <h1>VibroBug</h1>
      <button id="estop" class="danger">STOP</button>
    </header>

    <section id="joystick" aria-label="Drive joystick">
      <div id="stick"></div>
    </section>

    <section class="controls">
      <label>Max duty <input id="maxDuty" type="range" min="0.1" max="1" step="0.01" value="0.8"></label>
      <label>Left trim <input id="leftTrim" type="range" min="0.5" max="1.5" step="0.01" value="1"></label>
      <label>Right trim <input id="rightTrim" type="range" min="0.5" max="1.5" step="0.01" value="1"></label>
    </section>

    <section class="buttons">
      <button data-drive="forward">Forward</button>
      <button data-drive="left">Left</button>
      <button data-drive="right">Right</button>
      <button data-pattern="wiggle">Wiggle</button>
      <button data-pattern="explore">Explore</button>
      <button id="stop">Stop</button>
    </section>

    <section class="status">
      <div><span>Link</span><strong id="link">unknown</strong></div>
      <div><span>Left</span><strong id="left">0.00</strong></div>
      <div><span>Right</span><strong id="right">0.00</strong></div>
      <div><span>Age</span><strong id="age">-</strong></div>
    </section>
  </main>
  <script src="/app.js"></script>
</body>
</html>
)HTML";

const char STYLE_CSS[] PROGMEM = R"CSS(
* { box-sizing: border-box; }
html, body { margin: 0; min-height: 100%; background: #111; color: #f4f4f4; font-family: system-ui, -apple-system, Segoe UI, sans-serif; }
body { display: grid; place-items: center; padding: 16px; touch-action: none; }
main { width: min(100%, 560px); display: grid; gap: 14px; }
header { display: flex; align-items: center; justify-content: space-between; gap: 12px; }
h1 { margin: 0; font-size: 28px; letter-spacing: 0; }
button { min-height: 44px; border: 0; border-radius: 8px; background: #2c7be5; color: white; font: inherit; font-weight: 700; padding: 10px 14px; }
button:active { transform: translateY(1px); }
.danger, #stop { background: #d9363e; }
#joystick { position: relative; width: 100%; aspect-ratio: 1; max-height: 58vh; border: 2px solid #555; border-radius: 8px; background: radial-gradient(circle at center, #2b2b2b, #171717); overflow: hidden; touch-action: none; }
#joystick::before, #joystick::after { content: ""; position: absolute; background: #444; opacity: 0.8; }
#joystick::before { width: 2px; height: 100%; left: calc(50% - 1px); top: 0; }
#joystick::after { height: 2px; width: 100%; top: calc(50% - 1px); left: 0; }
#stick { position: absolute; width: 74px; height: 74px; border-radius: 50%; background: #f4c542; left: calc(50% - 37px); top: calc(50% - 37px); box-shadow: 0 5px 18px #000b; }
.controls { display: grid; gap: 10px; }
label { display: grid; gap: 4px; font-size: 14px; color: #ddd; }
input[type=range] { width: 100%; }
.buttons { display: grid; grid-template-columns: repeat(3, 1fr); gap: 8px; }
.status { display: grid; grid-template-columns: repeat(4, 1fr); gap: 8px; }
.status div { background: #222; border: 1px solid #333; border-radius: 8px; padding: 10px; display: grid; gap: 4px; min-width: 0; }
.status span { color: #aaa; font-size: 12px; }
.status strong { font-size: 16px; overflow-wrap: anywhere; }
@media (max-width: 420px) {
  body { padding: 10px; }
  .buttons { grid-template-columns: repeat(2, 1fr); }
  .status { grid-template-columns: repeat(2, 1fr); }
}
)CSS";

const char APP_JS[] PROGMEM = R"JS(
const joystick = document.querySelector("#joystick");
const stick = document.querySelector("#stick");
const link = document.querySelector("#link");
const leftOut = document.querySelector("#left");
const rightOut = document.querySelector("#right");
const ageOut = document.querySelector("#age");
let pointerActive = false;
let current = { throttle: 0, turn: 0 };
let sendTimer = null;

async function postJson(path, body = {}) {
  const response = await fetch(path, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body)
  });
  if (!response.ok) throw new Error(path);
  return response.json();
}

async function stop() {
  current = { throttle: 0, turn: 0 };
  centerStick();
  await postJson("/stop", {});
}

function centerStick() {
  stick.style.left = "calc(50% - 37px)";
  stick.style.top = "calc(50% - 37px)";
}

function updateJoystick(clientX, clientY) {
  const rect = joystick.getBoundingClientRect();
  const radius = Math.min(rect.width, rect.height) / 2;
  const cx = rect.left + rect.width / 2;
  const cy = rect.top + rect.height / 2;
  let x = (clientX - cx) / radius;
  let y = (clientY - cy) / radius;
  const mag = Math.hypot(x, y);
  if (mag > 1) {
    x /= mag;
    y /= mag;
  }
  stick.style.left = `${(x + 1) * 50}%`;
  stick.style.top = `${(y + 1) * 50}%`;
  stick.style.transform = "translate(-37px, -37px)";
  current = { throttle: Math.max(0, -y), turn: x };
}

async function sendDrive() {
  if (!pointerActive) return;
  await postJson("/drive", {
    mode: "drive",
    throttle: current.throttle,
    turn: current.turn,
    ttl_ms: 500
  }).catch(() => {});
}

function startSending() {
  clearInterval(sendTimer);
  sendTimer = setInterval(sendDrive, 75);
  sendDrive();
}

joystick.addEventListener("pointerdown", (event) => {
  pointerActive = true;
  joystick.setPointerCapture(event.pointerId);
  updateJoystick(event.clientX, event.clientY);
  startSending();
});

joystick.addEventListener("pointermove", (event) => {
  if (pointerActive) updateJoystick(event.clientX, event.clientY);
});

async function releasePointer() {
  if (!pointerActive) return;
  pointerActive = false;
  clearInterval(sendTimer);
  await stop().catch(() => {});
}

joystick.addEventListener("pointerup", releasePointer);
joystick.addEventListener("pointercancel", releasePointer);
document.querySelector("#estop").addEventListener("click", () => stop().catch(() => {}));
document.querySelector("#stop").addEventListener("click", () => stop().catch(() => {}));

document.querySelectorAll("[data-drive]").forEach((button) => {
  button.addEventListener("click", () => {
    const mode = button.dataset.drive;
    const commands = {
      forward: { throttle: 0.65, turn: 0 },
      left: { throttle: 0.55, turn: -0.7 },
      right: { throttle: 0.55, turn: 0.7 }
    };
    postJson("/drive", { mode: "drive", ...commands[mode], ttl_ms: 500 }).catch(() => {});
  });
});

document.querySelectorAll("[data-pattern]").forEach((button) => {
  button.addEventListener("click", () => {
    postJson("/drive", { mode: "pattern", name: button.dataset.pattern, intensity: 0.7, duration_ms: 3000 }).catch(() => {});
  });
});

async function pushConfig() {
  await postJson("/config", {
    max_duty: Number(document.querySelector("#maxDuty").value),
    left_trim: Number(document.querySelector("#leftTrim").value),
    right_trim: Number(document.querySelector("#rightTrim").value)
  }).catch(() => {});
}

document.querySelectorAll("input[type=range]").forEach((input) => input.addEventListener("change", pushConfig));

async function pollStatus() {
  try {
    const status = await fetch("/status").then((r) => r.json());
    link.textContent = "connected";
    leftOut.textContent = Number(status.left).toFixed(2);
    rightOut.textContent = Number(status.right).toFixed(2);
    ageOut.textContent = `${status.last_command_ms_ago} ms`;
  } catch {
    link.textContent = "disconnected";
  }
}

setInterval(pollStatus, 300);
pollStatus();
)JS";

