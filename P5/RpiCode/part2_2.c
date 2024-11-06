#include <stdio.h>
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
    request[7] = (crc >> 8);

    write(fd, request, 8);

    printf("Sent request: ");
    for (int i = 0; i < 8; i++) {
        printf("%02x ", request[i]);
    }
    printf("\n");
}

void read_response(int fd) {
    uint8_t response[8];
    int n = read(fd, response, 8);

    if (n > 0) {
        printf("Received reply: ");
        for (int i = 0; i < n; i++) {
            printf("%02x ", response[i]);
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

/*#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

uint16_t ModRTU_CRC(uint8_t buf[], int len) {
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

int open_serial(const char *device, int baudrate) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) return fd;
    
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) return -1;

    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) return -1;
    return fd;
}

void send_modbus_request(int fd, uint8_t address, uint8_t function, uint16_t reg, uint16_t value) {
    uint8_t request[8];
    request[0] = address;
    request[1] = function;
    request[2] = reg >> 8;
    request[3] = reg & 0xFF;
    request[4] = value >> 8;
    request[5] = value & 0xFF;
    
    uint16_t crc = ModRTU_CRC(request, 6);
    request[6] = crc & 0xFF;
    request[7] = crc >> 8;

    write(fd, request, 8);
}

int read_modbus_response(int fd, uint8_t *response, int len) {
    int total = 0;
    while (total < len) {
        int n = read(fd, response + total, len - total);
        if (n > 0) total += n;
        else if (n < 0) return -1;
    }
    return total;
}

void print_modbus_response(uint8_t *response, int length) {
    for (int i = 0; i < length; i++) {
        printf("%02X ", response[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Usage: %s <address> <function> <register> <value>\n", argv[0]);
        return 1;
    }

    uint8_t address = (uint8_t)strtoul(argv[1], NULL, 0);
    uint8_t function = (uint8_t)strtoul(argv[2], NULL, 0);
    uint16_t reg = (uint16_t)strtoul(argv[3], NULL, 0);
    uint16_t value = (uint16_t)strtoul(argv[4], NULL, 0);

    int fd = open_serial("/dev/ttyS0", B115200);
    if (fd < 0) {
        perror("Error opening serial port");
        return 1;
    }

    send_modbus_request(fd, address, function, reg, value);

    uint8_t response[8];
    int length = read_modbus_response(fd, response, 8);

    if (length < 0) {
        perror("Error reading response");
    } else {
        printf("Received reply: ");
        print_modbus_response(response, length);
    }

    close(fd);
    return 0;
}
*/