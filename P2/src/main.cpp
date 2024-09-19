#include <avr/io.h>
#include "encoder.h"
#include "Timer_msec.h"
#include "Arduino.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include "motor_driver.h"

Encoder encoder(PB4, PB3);
volatile int pulseCount = 0;
volatile float motorSpeedPPS = 0;
const int pulsesPerRevolution = 700;

//------Added for part 2-----
MotorDriver motor(PB1, PB2);
volatile float maxMotorSpeedPPS = 0;
volatile unsigned long startTime = 0;
volatile bool speedReached63Percent = false; 
volatile float maxSpeed_63 = 1436 * 0.63;
volatile int interruptCount = 0;
volatile int counter_ms = 0;


Timer_msec timer;

ISR(TIMER1_COMPA_vect) {
    counter_ms ++;
    pulseCount = encoder.readAndResetPulseCount();
    motorSpeedPPS = pulseCount * 100; //Changed for part 2
    float motorSpeedRPM = (motorSpeedPPS / pulsesPerRevolution) * 60;
    

    if (motorSpeedPPS > maxMotorSpeedPPS){
        maxMotorSpeedPPS = motorSpeedPPS;
    }

    if (!speedReached63Percent && motorSpeedPPS >= maxSpeed_63) {
        speedReached63Percent = true;
        Serial.println(counter_ms * 90); 
    }
    
    /*
    Serial.print("Speed: ");
    Serial.print(motorSpeedPPS);
    Serial.print(" PPS, ");
    Serial.print(motorSpeedRPM);
    Serial.print(" RPM");

    //------Added for part 2-----
    Serial.print("Max speed: ");
    Serial.print(maxMotorSpeedPPS);
    Serial.println(" PPS, ");*/
    
}


int main() {
    Serial.begin(9600);
    encoder.init();
    timer.init(10); //was timer.init(1000); in part 1
    sei();

    motor.init();
    motor.setSpeed(255);
    while(1){
        
    }


    return 0;            
}


ISR(PCINT0_vect) {
    encoder.position();
}
