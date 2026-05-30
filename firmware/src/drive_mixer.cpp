#include "drive_mixer.h"

#include <Arduino.h>

MotorCommand mixDrive(float throttle, float turn) {
  throttle = constrain(throttle, 0.0f, 1.0f);
  turn = constrain(turn, -1.0f, 1.0f);

  float left = throttle * (1.0f + turn);
  float right = throttle * (1.0f - turn);
  const float maxSide = max(left, right);

  if (maxSide > 1.0f) {
    left /= maxSide;
    right /= maxSide;
  }

  return {constrain(left, 0.0f, 1.0f), constrain(right, 0.0f, 1.0f)};
}

