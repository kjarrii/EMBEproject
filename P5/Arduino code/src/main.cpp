#include <Arduino.h>

void setup() {
    Serial.begin(115200, SERIAL_8N1);
}

void loop() {
    if (Serial.available() > 0) {
        char received = Serial.read();
        Serial.print("I received ");
        Serial.println(received);
    }
}
