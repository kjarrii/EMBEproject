#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>

class PController {
public:
    PController(float kp);
    
    int16_t calculate(int32_t setpoint, int32_t actual);

private:
    float kp;
};

#endif
