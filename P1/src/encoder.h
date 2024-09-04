#ifndef ENCODER_H
#define ENCODER_H

#include "digital_in.h"

class Encoder {
public:
    Encoder(uint8_t C1, uint8_t C2);
    void init();
    int position();

private:
    Digital_in C1;
    Digital_in C2;
    int posCount;
    bool C1LastVal;
};

#endif
