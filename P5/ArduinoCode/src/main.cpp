//===== Part 2.1 =====
//Code based on assignment 5.2
/*
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
*/


//===== Part 2.2 =====
#include <avr/io.h>
#include <util/delay.h>
#include <util/crc16.h>

#define MODBUS_ADDRESS 0x02
#define REGISTER_COUNT 10

uint16_t registers[REGISTER_COUNT];

uint16_t compute_crc(uint8_t *buf, uint8_t len) {
    uint16_t crc = 0xFFFF;
    for (uint8_t pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos];
        for (uint8_t i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void respond(uint8_t *response, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = response[i];
    }
}

void process_modbus_request(uint8_t *request, uint8_t length) {
    if (length < 8) return;
    if (request[0] != MODBUS_ADDRESS) return;

    int16_t crc = compute_crc(request, length - 2);
    if (crc != (request[length - 2] | (request[length - 1] << 8))) return;

    uint8_t response[8];
    response[0] = request[0];
    response[1] = request[1];

    if (request[1] == 0x03) {
        uint16_t reg_addr = (request[2] << 8) | request[3];
        if (reg_addr < REGISTER_COUNT) {
            response[2] = 0x02;
            response[3] = registers[reg_addr] >> 8;
            response[4] = registers[reg_addr] & 0xFF;
            crc = compute_crc(response, 5);
            response[5] = crc & 0xFF;
            response[6] = crc >> 8;
            respond(response, 7);
        }
    } else if (request[1] == 0x06) {
        uint16_t reg_addr = (request[2] << 8) | request[3];
        uint16_t value = (request[4] << 8) | request[5];
        if (reg_addr < REGISTER_COUNT) {
            registers[reg_addr] = value;
            for (uint8_t i = 0; i < 6; i++) response[i] = request[i];
            crc = compute_crc(response, 6);
            response[6] = crc & 0xFF;
            response[7] = crc >> 8;
            respond(response, 8);
        }
    }
}

int main() {
    // Set baud rate to 115200
    UBRR0H = 0;
    UBRR0L = 8;  // Baud rate register for 115200 at 16 MHz clock
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    while (1) {
        if (UCSR0A & (1 << RXC0)) {
            static uint8_t request[8];
            static uint8_t index = 0;
            request[index++] = UDR0;
            if (index >= 8) {
                process_modbus_request(request, index);
                index = 0;
            }
        }
    }
}



