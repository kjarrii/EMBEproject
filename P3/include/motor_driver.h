#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <stdint.h>
#include "digital_out.h"

class MotorDriver {
public:
    MotorDriver(DigitalOut& pin1, DigitalOut& pin2);
    
    void setSpeed(int16_t speed);
    void stop();

private:
    DigitalOut& motorPin1;
    DigitalOut& motorPin2;
};

#endif // MOTOR_DRIVER_H
