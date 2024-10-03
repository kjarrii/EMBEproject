#include <avr/io.h>
#include "motor_driver.h"
#include "encoder.h"
#include "controller.h"
#include "digital_out.h"
#include "digital_in.h"

int main() {

    DigitalOut motorPin1(&PORTB, PB0);
    DigitalOut motorPin2(&PORTB, PB1);
    MotorDriver motor(motorPin1, motorPin2);


    DigitalIn encoderA(&PINB, PB2);
    DigitalIn encoderB(&PINB, PB3);
    Encoder encoder(encoderA, encoderB);


    PController controller(0.5f);

    int32_t setpoint = 100; 

    while (true) {

        int32_t position = encoder.getCount();


        int16_t controlSignal = controller.calculate(setpoint, position);


        motor.setSpeed(controlSignal);
    }
}
