#ifndef ANALOG_OUT_H
#define ANALOG_OUT_H

#include <stdint.h>

class AnalogOut {
public:
    AnalogOut(uint8_t pwmPin); 
    void writePWM(uint8_t value);

private:
    uint8_t pwmPin;
};

#endif
