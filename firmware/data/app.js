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
  stick.style.transform = "none";
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
  await postJson("/drive", { mode: "drive", throttle: current.throttle, turn: current.turn, ttl_ms: 500 }).catch(() => {});
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
    const commands = {
      forward: { throttle: 0.65, turn: 0 },
      left: { throttle: 0.55, turn: -0.7 },
      right: { throttle: 0.55, turn: 0.7 }
    };
    postJson("/drive", { mode: "drive", ...commands[button.dataset.drive], ttl_ms: 500 }).catch(() => {});
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

