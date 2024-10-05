#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "analog_out.h"

class MotorDriver {
public:
    MotorDriver(AnalogOut& pin1, AnalogOut& pin2);  // Updated to take two direction pins and AnalogOut for PWM control
    void setSpeed(int16_t speed);    // Set motor speed using PWM
    void stop();                     // Stop the motor
    
private:
    AnalogOut& motorPin1;           // Motor direction pin 1
    AnalogOut& motorPin2;           // Motor direction pin 2
};

#endif
