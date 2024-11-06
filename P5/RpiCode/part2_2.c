/*#include <stdio.h>
#include <stdlib.h> 
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdint.h>
#include <string.h>

uint16_t compute_crc(uint8_t *buf, int len) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos];
        for (int i = 8; i != 0; i--) {
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

int open_serial(const char *device) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) return -1;

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    cfmakeraw(&options);
    tcsetattr(fd, TCSANOW, &options);
    return fd;
}

void send_request(int fd, uint8_t server, uint8_t func, uint16_t reg, uint16_t value) {
    uint8_t request[8];
    request[0] = server;
    request[1] = func;
    request[2] = reg >> 8;
    request[3] = reg & 0xFF;

    if (func == 0x06) { 
        request[4] = value >> 8;
        request[5] = value & 0xFF;
    } else {
        request[4] = 0x00;
        request[5] = 0x01;
    }

    uint16_t crc = compute_crc(request, 6);
    request[6] = crc & 0xFF; 
    request[7] = (crc >> 8) & 0xFF;

    write(fd, request, 8);

    printf("Sent request: ");
    for (int i = 0; i < 8; i++) {
        printf("%02X ", request[i]);
    }
    printf("\n");
}

void read_response(int fd) {
    uint8_t response[8];
    int n = read(fd, response, 8);

    if (n > 0) {
        printf("Received reply: ");
        for (int i = 0; i < n; i++) {
            printf("%02X ", response[i]);
        }
        printf("\n");

        if ((response[1] & 0x80) == 0x80) {
            printf("Exception Code: %d\n", response[2]);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <server> <function> <register> <value>\n", argv[0]);
        return -1;
    }

    uint8_t server = atoi(argv[1]);
    uint8_t func = atoi(argv[2]);
    uint16_t reg = atoi(argv[3]);
    uint16_t value = (func == 0x06) ? atoi(argv[4]) : 0;

    int fd = open_serial("/dev/ttyS0");
    if (fd == -1) {
        fprintf(stderr, "Error opening serial port.\n");
        return -1;
    }

    send_request(fd, server, func, reg, value);
    usleep(500000);
    read_response(fd);

    close(fd);
    return 0;
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

uint16_t compute_crc(uint8_t *buf, int len) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos];
        for (int i = 8; i != 0; i--) {
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

void send_request(int fd, uint8_t *request, int length) {
    write(fd, request, length);
}

int read_response(int fd, uint8_t *response, int length) {
    int bytes_read = 0;
    while (bytes_read < length) {
        int result = read(fd, &response[bytes_read], length - bytes_read);
        if (result < 1) return -1;
        bytes_read += result;
    }
    return bytes_read;
}

int main(int argc, char *argv[]) {
    if (argc != 5) return 1;

    uint8_t server = atoi(argv[1]);
    uint8_t function = atoi(argv[2]);
    uint16_t reg_addr = atoi(argv[3]);
    uint16_t value = atoi(argv[4]);

    int fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) return 1;

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    cfmakeraw(&options);
    tcsetattr(fd, TCSANOW, &options);

    uint8_t request[8];
    request[0] = server;
    request[1] = function;
    request[2] = reg_addr >> 8;
    request[3] = reg_addr & 0xFF;
    request[4] = value >> 8;
    request[5] = value & 0xFF;
    uint16_t crc = compute_crc(request, 6);
    request[6] = crc & 0xFF;
    request[7] = crc >> 8;

    send_request(fd, request, 8);

    uint8_t response[8];
    if (read_response(fd, response, 8) != 8) {
        close(fd);
        return 1;
    }

    for (int i = 0; i < 8; i++) printf("%02X ", response[i]);
    printf("\n");

    close(fd);
    return 0;
}

