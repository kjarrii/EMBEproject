#include <avr/io.h>
#include "encoder.h"
#include "Arduino.h"
#include <util/delay.h>
#include <avr/interrupt.h>

Encoder encoder(PB4, PB3);
int position = 0;

int main() {
    Serial.begin(115200); //Added for part 2
    encoder.init();
    //int prevPosition = 0; //Added for part 2
    sei(); //Added for part 3

    while (1) {

      //position = encoder.position();
      //Added for part 2=====================
      /*
      if (position != prevPosition) {
        Serial.println(encoder.position());
        prevPosition = position;
      }
      */
    }
    return 0;
}

//Added for part 3=====================
ISR(PCINT0_vect)
{
  Serial.println(encoder.position());
}
