#include <avr/io.h>
#include "encoder.h"
#include "Arduino.h"
#include <util/delay.h>

int main() {
    Serial.begin(9600);
    DDRB |= (1 << DDB5);
    PORTB &= ~(1 << PORTB5);

    Encoder encoder(PB4, PB3);
    encoder.init();
    int position = 0;

    while (1) {
      _delay_us(35);
      position = encoder.position();
      //Serial.println(encoder.position());
    }
    return 0;
}
