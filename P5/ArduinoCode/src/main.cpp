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
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define BAUD 115200
#define MYUBRR F_CPU/16/BAUD-1

void uart_init(unsigned int ubrr) {
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
}

void uart_transmit(unsigned char data) {
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = data;
}

unsigned char uart_receive() {
    while (!(UCSR0A & (1<<RXC0)));
    return UDR0;
}

unsigned int crc16(uint8_t *buf, uint8_t len) {
    uint16_t crc = 0xFFFF;
    for (uint8_t pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos];
        for (uint8_t i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else crc >>= 1;
        }
    }
    return crc;
}

void send_response(uint8_t *message, uint8_t length) {
    uint16_t crc = crc16(message, length);
    for (uint8_t i = 0; i < length; i++) uart_transmit(message[i]);
    uart_transmit(crc & 0xFF);
    uart_transmit((crc >> 8) & 0xFF);
}

void handle_modbus(uint8_t *msg) {
    uint8_t slave_id = msg[0];
    uint8_t func_code = msg[1];
    uint8_t reg_addr = msg[2];
    uint8_t value = msg[4];
    uint16_t crc_received = (msg[5] << 8) | msg[6];
    
    uint16_t crc_calculated = crc16(msg, 5);
    if (crc_received != crc_calculated) return;
    
    uint8_t response[8] = {slave_id, func_code, reg_addr, msg[3], value, msg[5], msg[6]};
    if (func_code == 0x06) {
        if (reg_addr == 0x01) OCR0A = value;
    } else if (func_code == 0x03) response[4] = OCR0A;
    
    send_response(response, 5);
}

int main(void) {
    uart_init(MYUBRR);
    DDRB = (1<<PB1);
    TCCR0A = (1<<COM0A1) | (1<<WGM00);
    TCCR0B = (1<<CS00);
    
    while (1) {
        uint8_t message[8];
        for (uint8_t i = 0; i < 8; i++) message[i] = uart_receive();
        handle_modbus(message);
    }
}
