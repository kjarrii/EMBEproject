
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "encoder.h"
#include "motor_driver.h"
#include "controller.h"
#include "PIcontroller.h"
#include "analog_out.h"
#include "digital_out.h"
#include <Arduino.h> 

Encoder encoder(PB4, PB3); 
volatile int pulseCount = 0;
volatile float motorSpeedPPS = 0;
const int pulsesPerRevolution = 700; 
volatile int timerCounter = 0;

AnalogOut motorPin1(PD5);
AnalogOut motorPin2(PD6);

MotorDriver motor(motorPin1, motorPin2);

PIController piController(0.5f, 0.1f, 0.01f); 
volatile int setpoint = 1000;

DigitalOut led(&PORTB, PB5); 
volatile int ledBlinkCounter = 0;
volatile bool ledState = false;

void initTimer1() {
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11);  
    OCR1A = 20000;
    TIMSK1 |= (1 << OCIE1A);
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
    PREOPERATIONAL,
    OPERATIONAL,
    STOP
};
SystemState currentState = INITIALIZATION;
SystemState lastState;

void transitionToState(SystemState newState) {
    lastState = currentState;
    currentState = newState;
    
    switch (newState) {
        case INITIALIZATION:
            Serial.println("State: Initialization");
            encoder.init();
            initTimer1();
            sei();
            led.write(0);
            motor.stop();
            Serial.println("Booted up, available commands are 'o', 'r' and 'p'");
            transitionToState(PREOPERATIONAL);
            break;

        case PREOPERATIONAL: {
            Serial.println("State: Pre-Operational");
            Serial.print("Enter Kp: ");
            while (Serial.available() == 0);
            double Kp = Serial.parseFloat();
            Serial.print("Enter Ti: ");
            while (Serial.available() == 0); 
            double Ti = Serial.parseFloat(); 
       
            piController = PIController(Kp, Ti, 0.01f); 

            Serial.println("Controller parameters updated.");
            Serial.println("Press o to go into the operational stage");
            break;
        }

        case OPERATIONAL:
            Serial.println("State: Operational");
            led.write(1);
            motor.setSpeed(100);
            break;

        case STOP:
            Serial.println("State: Stop");
            led.write(0);
            motor.stop();
    }
}


int main() {
    Serial.begin(9600);
    transitionToState(INITIALIZATION);
    initEstop();

    while (true) {
        if (Serial.available() > 0) {
            char command = Serial.read();

            if (command == 'o') {
                transitionToState(OPERATIONAL);
            } 
            else if (command == 'r') {
                transitionToState(lastState);
            }
            else if (command == 'p'){
                transitionToState(PREOPERATIONAL);
            }
        }
    }

    return 0;
}

ISR(INT0_vect) {
    //Estop
    if (currentState == OPERATIONAL){
        transitionToState(STOP);
    }
}

ISR(TIMER1_COMPA_vect) {
    pulseCount = encoder.readAndResetPulseCount();
    motorSpeedPPS = pulseCount * 100; 
    int16_t controlSignal = piController.update(setpoint, motorSpeedPPS);
    if (currentState == OPERATIONAL){
       motor.setSpeed(controlSignal);    
    }

    ledBlinkCounter++;
    if (currentState == PREOPERATIONAL && ledBlinkCounter >= 50) {
        ledState = !ledState;
        led.write(ledState);
        ledBlinkCounter = 0;
    } 
    else if (currentState == STOP && ledBlinkCounter >= 25) {
        ledState = !ledState;
        led.write(ledState);
        ledBlinkCounter = 0;
    }
}