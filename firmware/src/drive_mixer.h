#pragma once

struct MotorCommand {
  float left;
  float right;
};

MotorCommand mixDrive(float throttle, float turn);

