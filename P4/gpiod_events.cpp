#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    const char *chipname = "gpiochip0";
    unsigned int line_num = 17;
    int value;

    chip = gpiod_chip_open_by_name(chipname);
    line = gpiod_chip_get_line(chip, line_num);
    gpiod_line_request_input(line, "gpio_poll");

    while (1) {
        value = gpiod_line_get_value(line);
        printf("Pin value: %d\n", value);
        usleep(1000);
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    return 0;
}
