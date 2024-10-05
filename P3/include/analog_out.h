#ifndef ANALOG_OUT_H
#define ANALOG_OUT_H

#include <stdint.h>

class AnalogOut {
public:
    AnalogOut(uint8_t pwmPin);         // Constructor to initialize the PWM pin
    void writePWM(uint8_t value);      // Function to set the PWM value (0-255)

private:
    uint8_t pwmPin;
};

#endif
