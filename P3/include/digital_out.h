#ifndef DIGITAL_OUT_H
#define DIGITAL_OUT_H

#include <stdint.h>

class DigitalOut {
public:
    DigitalOut(volatile uint8_t* port, uint8_t pin);
    
    void write(uint8_t value);

private:
    volatile uint8_t* port;
    uint8_t pin;
};

#endif