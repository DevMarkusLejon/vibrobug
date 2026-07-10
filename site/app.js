const steps = [...document.querySelectorAll(".step")];
const stepList = document.querySelector("#stepList");
const progressValue = document.querySelector("#progressValue");
const ring = document.querySelector(".ring-fg");
const circumference = 2 * Math.PI * 52;

ring.style.strokeDasharray = `${circumference}`;

steps.forEach((step, index) => {
  const li = document.createElement("li");
  const link = document.createElement("a");
  link.href = `#${step.id}`;
  link.textContent = `${index + 1}. ${step.dataset.step}`;
  li.append(link);
  stepList.append(li);
});

const stepLinks = [...stepList.querySelectorAll("a")];

function setActiveStep(activeStep) {
  const accent = activeStep.dataset.accent || "#e3b341";
  document.documentElement.style.setProperty("--accent", accent);

  steps.forEach((step) => step.classList.toggle("active", step === activeStep));
  stepLinks.forEach((link) => link.classList.toggle("active", link.getAttribute("href") === `#${activeStep.id}`));
}

const observer = new IntersectionObserver((entries) => {
  const visible = entries
    .filter((entry) => entry.isIntersecting)
    .sort((a, b) => b.intersectionRatio - a.intersectionRatio)[0];

  if (visible) {
    setActiveStep(visible.target);
  }
}, {
  rootMargin: "-28% 0px -38% 0px",
  threshold: [0.2, 0.4, 0.6, 0.8]
});

steps.forEach((step) => observer.observe(step));

function updateProgress() {
  const scrollable = document.documentElement.scrollHeight - window.innerHeight;
  const progress = scrollable > 0 ? window.scrollY / scrollable : 0;
  const percent = Math.round(Math.max(0, Math.min(1, progress)) * 100);
  progressValue.textContent = `${percent}%`;
  ring.style.strokeDashoffset = `${circumference * (1 - percent / 100)}`;
}

document.addEventListener("scroll", updateProgress, { passive: true });
updateProgress();

const partDetails = {
  controller: "Small Wi-Fi controller, enough PWM pins, and a compact 21 x 17.8 mm footprint.",
  actuators: "Two independent vibration motors let the robot bias motion left and right instead of wandering randomly.",
  drivers: "Low-side MOSFET switches keep motor current out of the ESP32-C3 pins and save weight.",
  power: "A small 1S LiPo keeps mass low; the physical switch gives a hard cutoff during testing."
};

document.querySelectorAll(".part").forEach((button) => {
  button.addEventListener("click", () => {
    document.querySelectorAll(".part").forEach((part) => part.classList.remove("selected"));
    button.classList.add("selected");
    document.querySelector("#partDetail").textContent = partDetails[button.dataset.part];
  });
});

const throttle = document.querySelector("#throttle");
const turn = document.querySelector("#turn");
const leftMeter = document.querySelector("#leftMeter");
const rightMeter = document.querySelector("#rightMeter");
const leftValue = document.querySelector("#leftValue");
const rightValue = document.querySelector("#rightValue");
const jsonPreview = document.querySelector("#jsonPreview");

function mixDrive(throttleValue, turnValue) {
  let left = throttleValue * (1 + turnValue);
  let right = throttleValue * (1 - turnValue);
  const maxSide = Math.max(left, right);

  if (maxSide > 1) {
    left /= maxSide;
    right /= maxSide;
  }

  return {
    left: Math.max(0, Math.min(1, left)),
    right: Math.max(0, Math.min(1, right))
  };
}

function updateMixer() {
  const throttleValue = Number(throttle.value);
  const turnValue = Number(turn.value);
  const mixed = mixDrive(throttleValue, turnValue);

  leftMeter.value = mixed.left;
  rightMeter.value = mixed.right;
  leftValue.textContent = mixed.left.toFixed(2);
  rightValue.textContent = mixed.right.toFixed(2);
  jsonPreview.textContent = JSON.stringify({
    mode: "drive",
    throttle: Number(throttleValue.toFixed(2)),
    turn: Number(turnValue.toFixed(2))
  }, null, 2);
}

throttle.addEventListener("input", updateMixer);
turn.addEventListener("input", updateMixer);
updateMixer();

const snippets = {
  drive: `curl -X POST http://192.168.4.1/drive \\
  -H "Content-Type: application/json" \\
  -d '{"mode":"drive","throttle":0.7,"turn":0.3}'`,
  direct: `curl -X POST http://192.168.4.1/drive \\
  -H "Content-Type: application/json" \\
  -d '{"mode":"direct","left":0.65,"right":0.30}'`,
  pattern: `curl -X POST http://192.168.4.1/drive \\
  -H "Content-Type: application/json" \\
  -d '{"mode":"pattern","name":"wiggle","intensity":0.7,"duration_ms":3000}'`,
  status: `curl http://192.168.4.1/status`
};

const apiSnippet = document.querySelector("#apiSnippet");

function setSnippet(key) {
  apiSnippet.textContent = snippets[key];
  document.querySelectorAll(".tab").forEach((tab) => tab.classList.toggle("active", tab.dataset.api === key));
}

document.querySelectorAll(".tab").forEach((tab) => {
  tab.addEventListener("click", () => setSnippet(tab.dataset.api));
});
setSnippet("drive");

