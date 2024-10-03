#include "motor_driver.h"

MotorDriver::MotorDriver(DigitalOut& pin1, DigitalOut& pin2)
    : motorPin1(pin1), motorPin2(pin2) {}

void MotorDriver::setSpeed(int16_t speed) {
    if (speed > 0) {
        motorPin1.write(1);
        motorPin2.write(0);
    } else if (speed < 0) {
        motorPin1.write(0);
        motorPin2.write(1);
    } else {
        stop();
    }
}

void MotorDriver::stop() {
    motorPin1.write(0);
    motorPin2.write(0);
}
