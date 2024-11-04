#include <iostream>
#include <iomanip>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>

uint16_t calculate_crc(uint8_t *buffer, int length);
void send_modbus_message(int fd, uint8_t server_id, uint8_t function_code, uint16_t register_address, uint16_t value);

int main(int argc, char *argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: ./modbus <server_id> <function_code> <register_address> <value>\n";
        return 1;
    }

    uint8_t server_id = std::stoi(argv[1]);
    uint8_t function_code = std::stoi(argv[2]);
    uint16_t register_address = std::stoi(argv[3]);
    uint16_t value = std::stoi(argv[4]);

    int fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        std::cerr << "Error opening serial port\n";
        return 1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= CREAD | CLOCAL;
    tcsetattr(fd, TCSANOW, &options);

    send_modbus_message(fd, server_id, function_code, register_address, value);

    close(fd);
    return 0;
}

uint16_t calculate_crc(uint8_t *buffer, int length) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < length; pos++) {
        crc ^= (uint16_t)buffer[pos];
        for (int i = 8; i != 0; i--) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void send_modbus_message(int fd, uint8_t server_id, uint8_t function_code, uint16_t register_address, uint16_t value) {
    uint8_t message[8];
    message[0] = server_id;
    message[1] = function_code;
    message[2] = register_address >> 8;
    message[3] = register_address & 0xFF;
    message[4] = value >> 8;
    message[5] = value & 0xFF;
    uint16_t crc = calculate_crc(message, 6);
    message[6] = crc & 0xFF;
    message[7] = crc >> 8;

    write(fd, message, 8);

    uint8_t response[8];
    int bytes_read = read(fd, response, 8);
    if (bytes_read == 8) {
        std::cout << "Received reply: ";
        for (int i = 0; i < 8; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)response[i] << " ";
        }
        std::cout << std::dec << "\n";
    } else {
        std::cerr << "Error: Incomplete or no response received.\n";
    }
}
