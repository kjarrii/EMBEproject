#include "digital_out.h"
#include <avr/io.h>

DigitalOut::DigitalOut(volatile uint8_t* port, uint8_t pin)
    : port(port), pin(pin) {
    // Set pin as output
    *(port - 1) |= (1 << pin);
}

void DigitalOut::write(uint8_t value) {
    if (value) {
        *port |= (1 << pin);
    } else {
        *port &= ~(1 << pin);
    }
}
