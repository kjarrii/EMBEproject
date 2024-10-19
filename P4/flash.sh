#!/bin/bash
CHIP="gpiochip0"
LINE=17

gpioset $CHIP $LINE=0
COUNTER=0

while [ $COUNTER -lt 100000 ]; do
    gpioset $CHIP $LINE=1
    let COUNTER=COUNTER+1
    gpioset $CHIP $LINE=0
done
