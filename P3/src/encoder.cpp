#include "encoder.h"

Encoder::Encoder(DigitalIn& channelA, DigitalIn& channelB)
    : encA(channelA), encB(channelB), count(0), lastStateA(false) {}

int32_t Encoder::getCount() {
    return count;
}

void Encoder::reset() {
    count = 0;
}

void Encoder::update() {
    bool currentStateA = encA.read();
    if (currentStateA != lastStateA) {
        if (encB.read() != currentStateA) {
            count++;
        } else {
            count--;
        }
    }
    lastStateA = currentStateA;
}
