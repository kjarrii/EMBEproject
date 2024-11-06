#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

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

int setup_serial(const char *device, int baudrate) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) return -1;

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

void print_bytes(const char *label, uint8_t *data, int length) {
    printf("%s:", label);
    for (int i = 0; i < length; i++) {
        printf(" %02x", data[i]);
    }
    printf("\n");
}

void send_modbus_request(int fd, uint8_t address, uint8_t function, uint16_t reg, uint16_t value) {
    uint8_t request[8];
    request[0] = address;
    request[1] = function;
    request[2] = reg >> 8;
    request[3] = reg & 0xFF;
    request[4] = value >> 8;
    request[5] = value & 0xFF;

    uint16_t crc = compute_crc(request, 6);
    request[6] = crc & 0xFF;
    request[7] = crc >> 8;

    print_bytes("Sent request", request, 8);
    write(fd, request, 8);
}

void read_modbus_response(int fd, uint8_t *response, int length) {
    int bytes_read = 0;
    while (bytes_read < length) {
        bytes_read += read(fd, response + bytes_read, length - bytes_read);
    }
    print_bytes("Received reply", response, length);
}

int main(int argc, char *argv[]) {
    if (argc < 5) return 1;

    int address = atoi(argv[1]);
    int function = atoi(argv[2]);
    int reg = atoi(argv[3]);
    int value = atoi(argv[4]);

    int fd = setup_serial("/dev/ttyS0", B115200);
    if (fd == -1) return 1;

    uint8_t response[8];

    if (function == 6) {
        send_modbus_request(fd, address, function, reg, value);
        read_modbus_response(fd, response, 8);
    } else if (function == 3) {
        send_modbus_request(fd, address, function, reg, 1);
        read_modbus_response(fd, response, 7);
    }

    close(fd);
    return 0;
}
