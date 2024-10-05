
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "encoder.h"
#include "motor_driver.h"
#include "controller.h"
#include "analog_out.h"
#include "digital_out.h"
#include <Arduino.h>  // For serial debugging

// Encoder setup
Encoder encoder(PB4, PB3); 
volatile int pulseCount = 0;
volatile float motorSpeedPPS = 0;
const int pulsesPerRevolution = 700; 
volatile int timerCounter = 0;

// Motor control pins
AnalogOut motorPin1(PD5);    // Motor direction pin 1
AnalogOut motorPin2(PD6);    // Motor direction pin 2

MotorDriver motor(motorPin1, motorPin2);  // Use both direction pins and PWM

// Proportional controller setup
PController pController(0.5f);  // P controller with a proportional gain of 0.05
volatile int setpoint = 1400;     // Desired speed in pulses per second (PPS)

DigitalOut led(&PORTB, PB5); 

// Timer1 setup function for the encoder 10ms
void initTimer1() {
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11);  
    OCR1A = 20000;
    TIMSK1 |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
    pulseCount = encoder.readAndResetPulseCount();
    motorSpeedPPS = pulseCount * 100; 
    int16_t controlSignal = pController.update(setpoint, motorSpeedPPS);
    //motor.setSpeed(controlSignal);    
}

ISR(PCINT0_vect) {
    encoder.position();
}

void initEstop() {
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2); 
    EICRA |= (1 << ISC01);
    EIMSK |= (1 << INT0);
}

enum SystemState {
    INITIALIZATION,
    OPERATIONAL
};
SystemState currentState = INITIALIZATION;

void transitionToState(SystemState newState) {
    currentState = newState;
    
    switch (newState) {
        case INITIALIZATION:
            Serial.println("State: Initialization");
            encoder.init();
            initTimer1();  // Initialize Timer1 for regular 10ms interrupts and PWM
            sei();  // Enable global interrupts
            led.write(0);
            motor.setSpeed(0);
            break;
        case OPERATIONAL:
            Serial.println("State: Operational");
            led.write(1);
            motor.setSpeed(100);
            break;
    }
}

int main() {
    Serial.begin(9600);
    transitionToState(INITIALIZATION);
    initEstop();

    while (true) {
        if (Serial.available() > 0) {
            char command = Serial.read();

            if (command == 'i') {
                transitionToState(INITIALIZATION);
            } else if (command == 'o') {
                transitionToState(OPERATIONAL);
            } else if (command == 'r') {
                //Reset command
                transitionToState(INITIALIZATION);
            }
        }
    }

    return 0;
}

ISR(INT0_vect) {
    //Estop
    if (currentState != INITIALIZATION){
        transitionToState(INITIALIZATION);
    }
}
