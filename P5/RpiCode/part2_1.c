#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

int setupSerial(const char* port) {
    int file = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (file == -1) {
        perror("UART: Failed to open the file.\n");
        return -1;
    }
    
    struct termios options;
    tcgetattr(file, &options);
    options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
    options.c_iflag = IGNPAR | ICRNL;
    tcflush(file, TCIFLUSH);
    tcsetattr(file, TCSANOW, &options);
    return file;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s [port] [character]\n", argv[0]);
        return -2;
    }

    const char* port = argv[1];
    char message = argv[2][0];
    int file = setupSerial(port);
    if (file == -1) return -1;

    int count = write(file, &message, 1);
    if (count < 0) {
        perror("Failed to write to the output\n");
        close(file);
        return -1;
    }

    usleep(100000);
    char receive[100];
    count = read(file, receive, sizeof(receive) - 1);
    if (count < 0) {
        perror("Failed to read from the input\n");
    } else if (count == 0) {
        printf("No data available to read!\n");
    } else {
        receive[count] = '\0';
        printf("Received from %s: %s\n", port, receive);
    }

    close(file);
    return 0;
}

