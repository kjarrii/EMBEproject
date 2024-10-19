#include "analog_out.h"
#include <avr/io.h>

AnalogOut::AnalogOut(uint8_t pwmPin) : pwmPin(pwmPin) {
    DDRD |= (1 << pwmPin);

    TCCR0A |= (1 << WGM00) | (1 << WGM01); 
    if (pwmPin == PD6) {
        TCCR0A |= (1 << COM0A1);           
    } else if (pwmPin == PD5) {
        TCCR0A |= (1 << COM0B1); 
    }
    TCCR0B |= (1 << CS01) | (1 << CS00);
}

void AnalogOut::writePWM(uint8_t value) {
    if (pwmPin == PD6) {
        OCR0A = value;
    } else if (pwmPin == PD5) {
        OCR0B = value;
    }
}
