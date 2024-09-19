#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <avr/io.h>
#include "digital_out.h"

class MotorDriver {
public:
    MotorDriver(uint8_t pinForward, uint8_t pinReverse);
    void init();
    void setSpeed(int speed); 
    void stop();

private:
    Digital_out pinF;
    Digital_out pinR;
};

#endif
