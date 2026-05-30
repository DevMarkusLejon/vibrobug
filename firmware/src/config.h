#pragma once

#include <Arduino.h>

// TODO: Verify these labels against the exact Seeed XIAO ESP32-C3 board package
// and the chosen motor driver wiring before connecting motors.
#ifndef D2
#define D2 4
#endif

#ifndef D3
#define D3 5
#endif

#ifndef D0
#define D0 2
#endif

constexpr uint8_t LEFT_MOTOR_PWM_PIN = D2;
constexpr uint8_t RIGHT_MOTOR_PWM_PIN = D3;
constexpr uint8_t BATTERY_ADC_PIN = D0;

constexpr uint8_t LEFT_PWM_CHANNEL = 0;
constexpr uint8_t RIGHT_PWM_CHANNEL = 1;
constexpr uint32_t PWM_FREQ_HZ = 20000;
constexpr uint8_t PWM_RES_BITS = 8;

constexpr uint32_t COMMAND_TIMEOUT_MS = 500;
constexpr float MAX_DUTY = 0.80f;
constexpr float RAMP_RATE_PER_SECOND = 2.0f;
constexpr float START_BOOST_DUTY = 0.85f;
constexpr uint32_t START_BOOST_MS = 80;
constexpr float MIN_EFFECTIVE_DUTY = 0.20f;

constexpr const char *AP_SSID_PREFIX = "VibroBug";
constexpr const char *AP_PASSWORD = "";
constexpr uint16_t HTTP_PORT = 80;

struct RuntimeConfig {
  float maxDuty = MAX_DUTY;
  float rampRatePerSecond = RAMP_RATE_PER_SECOND;
  float leftTrim = 1.0f;
  float rightTrim = 1.0f;
};

