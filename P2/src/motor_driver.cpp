#include "Motor_driver.h"

MotorDriver::MotorDriver(uint8_t pinForward, uint8_t pinReverse) : pinF(pinForward), pinR(pinReverse) {}

void MotorDriver::init() {
    pinF.init();
    pinR.init();
}

void MotorDriver::setSpeed(int speed) {
    if (speed > 0) {
        pinF.set_hi();
        pinR.set_lo();
    } else if (speed < 0) {
        pinF.set_lo();
        pinR.set_hi();
    } else {
        stop();
    }
}

void MotorDriver::stop() {
    pinF.set_lo();
    pinR.set_lo();
}
