#include "analog_out.h"
#include <avr/io.h>

AnalogOut::AnalogOut(uint8_t pwmPin) : pwmPin(pwmPin) {
    // Set the pin as an output
    DDRD |= (1 << pwmPin);  // Assuming PD5 or PD6

    // Set up Timer0 for Fast PWM
    TCCR0A |= (1 << WGM00) | (1 << WGM01);  // Fast PWM mode
    if (pwmPin == PD6) {
        TCCR0A |= (1 << COM0A1);            // Non-inverting mode for OC0A (PD6)
    } else if (pwmPin == PD5) {
        TCCR0A |= (1 << COM0B1);            // Non-inverting mode for OC0B (PD5)
    }
    TCCR0B |= (1 << CS01) | (1 << CS00);    // Prescaler = 64
}

void AnalogOut::writePWM(uint8_t value) {
    if (pwmPin == PD6) {
        OCR0A = value;  // Set PWM value for PD6 (OC0A)
    } else if (pwmPin == PD5) {
        OCR0B = value;  // Set PWM value for PD5 (OC0B)
    }
}
