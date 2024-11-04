#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdint.h>
#include <cstdlib>

uint16_t crc16(uint8_t *buf, int len) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos];
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else crc >>= 1;
        }
    }
    return crc;
}

void send_modbus_message(int file, uint8_t id, uint8_t func, uint8_t reg, uint8_t value) {
    uint8_t message[8] = {id, func, 0x00, reg, 0x00, value};
    uint16_t crc = crc16(message, 6);
    message[6] = crc & 0xFF;
    message[7] = (crc >> 8) & 0xFF;
    write(file, message, 8);
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

    uint8_t id = atoi(argv[1]);
    uint8_t func = atoi(argv[2]);
    uint8_t reg = atoi(argv[3]);
    uint8_t value = atoi(argv[4]);

    send_modbus_message(file, id, func, reg, value);

    uint8_t response[8];
    read(file, response, 8);
    close(file);
    return 0;
}
