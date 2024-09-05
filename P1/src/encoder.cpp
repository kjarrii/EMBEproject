#include "encoder.h"
#include "digital_out.h"
#include <avr/interrupt.h>

Encoder::Encoder(uint8_t C1, uint8_t C2) : C1(C1), C2(C2), posCount(), C1LastVal(false), led(5) {}

void Encoder::init() {
    C1.init();
    C2.init();
    C1LastVal = C1.is_hi();
    posCount = 0;
    led.init();
    // Added for part 3======
    PCICR |= (1 << PCIE0);
    PCMSK0 |= (1 << PCINT4);           
}

int Encoder::position() {
    bool C1Val = C1.is_hi();
    bool C2Val = C2.is_hi();
    
    if (C1Val != C1LastVal) {
        if (C1Val != C2Val) {
            posCount++;
        } else {
            posCount--;
        }
        led.toggle();
    }
    C1LastVal = C1Val;
    return posCount;
}

