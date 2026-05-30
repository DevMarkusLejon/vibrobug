#include "motor_driver.h"

void MotorDriver::begin() {
  ledcSetup(LEFT_PWM_CHANNEL, PWM_FREQ_HZ, PWM_RES_BITS);
  ledcSetup(RIGHT_PWM_CHANNEL, PWM_FREQ_HZ, PWM_RES_BITS);
  ledcAttachPin(LEFT_MOTOR_PWM_PIN, LEFT_PWM_CHANNEL);
  ledcAttachPin(RIGHT_MOTOR_PWM_PIN, RIGHT_PWM_CHANNEL);

  lastCommandMs_ = millis();
  lastUpdateMs_ = millis();
  stop();
}

void MotorDriver::setRaw(float left, float right) {
  const uint32_t now = millis();
  const float nextLeft = clampDuty(left * config_.leftTrim);
  const float nextRight = clampDuty(right * config_.rightTrim);

  if (output_.left <= 0.0f && nextLeft > 0.0f) {
    leftBoostUntilMs_ = now + START_BOOST_MS;
  }
  if (output_.right <= 0.0f && nextRight > 0.0f) {
    rightBoostUntilMs_ = now + START_BOOST_MS;
  }

  target_ = {nextLeft, nextRight};
  lastCommandMs_ = now;
}

void MotorDriver::stop() {
  target_ = {0.0f, 0.0f};
  output_ = {0.0f, 0.0f};
  leftBoostUntilMs_ = 0;
  rightBoostUntilMs_ = 0;
  lastCommandMs_ = millis();
  applyOutputs(0.0f, 0.0f);
}

void MotorDriver::update() {
  const uint32_t now = millis();

  if (now - lastCommandMs_ > COMMAND_TIMEOUT_MS) {
    target_ = {0.0f, 0.0f};
    output_ = {0.0f, 0.0f};
    leftBoostUntilMs_ = 0;
    rightBoostUntilMs_ = 0;
    applyOutputs(0.0f, 0.0f);
    lastUpdateMs_ = now;
    return;
  }

  const float deltaSeconds = (now - lastUpdateMs_) / 1000.0f;
  lastUpdateMs_ = now;

  output_.left = rampToward(output_.left, target_.left, deltaSeconds);
  output_.right = rampToward(output_.right, target_.right, deltaSeconds);

  float appliedLeft = output_.left;
  float appliedRight = output_.right;

  if (appliedLeft > 0.0f && appliedLeft < MIN_EFFECTIVE_DUTY) {
    appliedLeft = MIN_EFFECTIVE_DUTY;
  }
  if (appliedRight > 0.0f && appliedRight < MIN_EFFECTIVE_DUTY) {
    appliedRight = MIN_EFFECTIVE_DUTY;
  }

  if (leftBoostUntilMs_ > now && target_.left > 0.0f) {
    appliedLeft = max(appliedLeft, clampDuty(START_BOOST_DUTY));
  }
  if (rightBoostUntilMs_ > now && target_.right > 0.0f) {
    appliedRight = max(appliedRight, clampDuty(START_BOOST_DUTY));
  }

  applyOutputs(appliedLeft, appliedRight);
}

void MotorDriver::setRuntimeConfig(const RuntimeConfig &config) {
  config_ = config;
  config_.maxDuty = constrain(config_.maxDuty, 0.0f, 1.0f);
  config_.rampRatePerSecond = constrain(config_.rampRatePerSecond, 0.1f, 10.0f);
  config_.leftTrim = constrain(config_.leftTrim, 0.0f, 1.5f);
  config_.rightTrim = constrain(config_.rightTrim, 0.0f, 1.5f);
  target_.left = clampDuty(target_.left);
  target_.right = clampDuty(target_.right);
}

MotorCommand MotorDriver::target() const { return target_; }

MotorCommand MotorDriver::output() const { return output_; }

uint32_t MotorDriver::lastCommandAgeMs() const { return millis() - lastCommandMs_; }

RuntimeConfig MotorDriver::runtimeConfig() const { return config_; }

float MotorDriver::clampDuty(float value) const {
  return constrain(value, 0.0f, config_.maxDuty);
}

float MotorDriver::rampToward(float current, float target, float deltaSeconds) const {
  const float maxStep = config_.rampRatePerSecond * max(deltaSeconds, 0.0f);

  if (target > current) {
    return min(target, current + maxStep);
  }
  return max(target, current - maxStep);
}

void MotorDriver::writeMotor(uint8_t channel, float duty) const {
  const uint32_t maxPwm = (1UL << PWM_RES_BITS) - 1UL;
  const uint32_t pwm = static_cast<uint32_t>(constrain(duty, 0.0f, 1.0f) * maxPwm + 0.5f);
  ledcWrite(channel, pwm);
}

void MotorDriver::applyOutputs(float left, float right) {
  writeMotor(LEFT_PWM_CHANNEL, left);
  writeMotor(RIGHT_PWM_CHANNEL, right);
}
