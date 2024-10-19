#include "motor_driver.h"

MotorDriver::MotorDriver(AnalogOut& pin1, AnalogOut& pin2) 
    : motorPin1(pin1), motorPin2(pin2) {}

void MotorDriver::setSpeed(int16_t speed) {
    if (speed > 0) {
        motorPin1.writePWM(speed);
        motorPin2.writePWM(0);
    } else if (speed < 0) {
        motorPin1.writePWM(0);
        motorPin2.writePWM(speed);
    } else {
        stop();
    }
}

void MotorDriver::stop() {
    motorPin1.writePWM(0);
    motorPin2.writePWM(0);
}
