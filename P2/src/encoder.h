#include "digital_in.h"
#include <avr/io.h>

class Encoder {
public:
    Encoder(uint8_t C1, uint8_t C2);
    void init();
    int position();
    int readAndResetPulseCount();
private:
    Digital_in C1, C2;
    int posCount;
    bool C1LastVal;
};
