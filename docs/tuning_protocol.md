# Tuning Protocol

The mechanics dominate this robot. Change one variable at a time and record a short video for every configuration.

## Phase A: One-Motor Sanity Test

Goal: verify that vibration plus angled legs produces motion.

Acceptance:

- Robot moves at least 20 cm on a smooth floor within 10 seconds.
- Direction is biased rather than purely stationary spinning.

Suggested procedure:

1. Install only the left motor or run direct mode with the right motor at zero.
2. Test 30%, 50%, 70%, and 80% duty.
3. Repeat with the right motor.
4. If it does not move, change leg angle or stiffness before changing firmware.

## Phase B: Two-Motor Direct Control

Goal: verify differential actuation produces repeatable turn bias.

Acceptance:

- Left motor stronger causes repeatable right-ish curve or spin.
- Right motor stronger causes repeatable left-ish curve or spin.
- Both motors causes forward-ish motion.

Test commands:

```bash
curl -X POST http://192.168.4.1/drive -H "Content-Type: application/json" -d '{"mode":"direct","left":0.7,"right":0.2}'
curl -X POST http://192.168.4.1/drive -H "Content-Type: application/json" -d '{"mode":"direct","left":0.2,"right":0.7}'
curl -X POST http://192.168.4.1/drive -H "Content-Type: application/json" -d '{"mode":"direct","left":0.6,"right":0.6}'
```

## Phase C: Web Control

Goal: drive from a phone browser.

Acceptance:

- Control page loads from robot AP.
- Joystick changes left/right motor outputs.
- Touch release stops motors.
- Command timeout stops motors if the page is closed.

## Phase D: Cat-Safe Enclosure Prototype

Goal: enclose the electronics without killing locomotion.

Acceptance:

- No exposed electronics or detachable tiny parts.
- Still moves and turns after enclosure is mounted.
- Can survive light batting/tapping.

Do not leave the prototype unsupervised with a cat.

