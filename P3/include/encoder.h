#ifndef ENCODER_H
#define ENCODER_H

#include "digital_in.h"

class Encoder {
public:
    Encoder(DigitalIn& channelA, DigitalIn& channelB);
    int32_t getCount(); 
    void reset();

    void update(); 
private:
    DigitalIn& encA;
    DigitalIn& encB;
    int32_t count;
    bool lastStateA;
};

#endif 
