#ifndef TIMER_MSEC_H
#define TIMER_MSEC_H

#include <avr/io.h>

class Timer_msec {
public:
    Timer_msec();
    void init(uint32_t period_ms);
};

#endif
