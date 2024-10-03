#ifndef DIGITAL_IN_H
#define DIGITAL_IN_H

#include <stdint.h>

class DigitalIn {
public:
    DigitalIn(volatile uint8_t* pinReg, uint8_t pin);
    
    uint8_t read();

private:
    volatile uint8_t* pinReg;
    uint8_t pin;
};

#endif
