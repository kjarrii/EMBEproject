#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "analog_out.h"

class MotorDriver {
public:
    MotorDriver(AnalogOut& pin1, AnalogOut& pin2);
    void setSpeed(int16_t speed);
    void stop();
    
private:
    AnalogOut& motorPin1;
    AnalogOut& motorPin2;
};

#endif
