#include <avr/io.h>
#include "digital_in.h"

Digital_in::Digital_in(uint8_t pin) {
    pinMask = (1 << pin);
}
void Digital_in::init() {
    DDRB &= ~pinMask;
    PORTB |= pinMask;
}

bool Digital_in::is_hi() {
    return PINB & pinMask;
}

bool Digital_in::is_lo() {
    return !(PINB & pinMask);
}
