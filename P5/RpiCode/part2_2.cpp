#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdint.h>
#include <cstdlib>


uint16_t calculateCRC(uint8_t *buffer, uint8_t length) {
    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < length; i++) {
        crc ^= buffer[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 1) crc = (crc >> 1) ^ 0xA001;
            else crc = crc >> 1;
        }
    }
    return crc;
}

void sendModbusMessage(int file, uint8_t server, uint8_t function, uint16_t reg, uint16_t value) {
    uint8_t request[8] = {server, function, (uint8_t)(reg >> 8), (uint8_t)reg, (uint8_t)(value >> 8), (uint8_t)value};
    uint16_t crc = calculateCRC(request, 6);
    request[6] = (uint8_t)(crc & 0xFF);
    request[7] = (uint8_t)(crc >> 8);
    write(file, request, 8);
}

int main(int argc, char *argv[]) {
    int file = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (file < 0) return -1;

    struct termios options;
    tcgetattr(file, &options);
    options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
    options.c_iflag = IGNPAR;
    tcflush(file, TCIFLUSH);
    tcsetattr(file, TCSANOW, &options);

    uint8_t server = atoi(argv[1]);
    uint8_t function = atoi(argv[2]);
    uint16_t reg = atoi(argv[3]);
    uint16_t value = atoi(argv[4]);

    sendModbusMessage(file, server, function, reg, value);
    usleep(100000);

    uint8_t response[8];
    int count = read(file, response, 8);
    if (count > 0) {
        for (int i = 0; i < count; i++) printf("%02X ", response[i]);
        printf("\n");
    }

    close(file);
    return 0;
}
