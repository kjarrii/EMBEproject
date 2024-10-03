#include "controller.h"

PController::PController(float kp) : kp(kp) {}

int16_t PController::calculate(int32_t setpoint, int32_t actual) {
    int32_t error = setpoint - actual;
    return static_cast<int16_t>(kp * error);
}
