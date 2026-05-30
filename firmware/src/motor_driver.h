#pragma once

#include <Arduino.h>

#include "config.h"
#include "drive_mixer.h"

class MotorDriver {
public:
  void begin();
  void setRaw(float left, float right);
  void stop();
  void update();

  void setRuntimeConfig(const RuntimeConfig &config);
  MotorCommand target() const;
  MotorCommand output() const;
  uint32_t lastCommandAgeMs() const;
  RuntimeConfig runtimeConfig() const;

private:
  float clampDuty(float value) const;
  float rampToward(float current, float target, float deltaSeconds) const;
  void writeMotor(uint8_t channel, float duty) const;
  void applyOutputs(float left, float right);

  RuntimeConfig config_;
  MotorCommand target_{0.0f, 0.0f};
  MotorCommand output_{0.0f, 0.0f};
  uint32_t lastCommandMs_ = 0;
  uint32_t lastUpdateMs_ = 0;
  uint32_t leftBoostUntilMs_ = 0;
  uint32_t rightBoostUntilMs_ = 0;
};

