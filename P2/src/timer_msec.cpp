#include "Timer_msec.h"
#include <avr/io.h>

Timer_msec::Timer_msec() {
}

//Timer based on assignment 2.1
void Timer_msec::init(uint32_t period_ms) {
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    OCR1A = period_ms * uint32_t{16000} / 1024 - 1;
    TCCR1B |= (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);
    TCCR1B |= (1 << CS12) | (1 << CS10);
}
