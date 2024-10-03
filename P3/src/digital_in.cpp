#include "digital_in.h"
#include <avr/io.h>

DigitalIn::DigitalIn(volatile uint8_t* pinReg, uint8_t pin)
    : pinReg(pinReg), pin(pin) {
    // Set pin as input
    *(pinReg - 1) &= ~(1 << pin);
}

uint8_t DigitalIn::read() {
    return (*pinReg & (1 << pin)) ? 1 : 0;
}
