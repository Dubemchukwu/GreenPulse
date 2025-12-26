#include "Actuator.h"
#include <Arduino.h>

// TODO: most remove this line please 💩 -> 5, remove it anywhere you see it
#include <cstdint>

uint8_t motorInputOne = 0x13;
uint8_t motorInputTwo = 0x14;
uint8_t motorStandby = 0x12;

void initializeActuator() {
    // Initialize Actuator(Pump) and Set it to Stop, to avoid misharps
    pinMode(motorInputOne, OUTPUT);
    pinMode(motorInputTwo, OUTPUT);
    pinMode(motorStandby, OUTPUT);
    stopActuator();
}

void moveActuator(){
    digitalWrite(motorStandby, HIGH);
    digitalWrite(motorInputOne, HIGH);
    digitalWrite(motorInputTwo, LOW);
}

void stopActuator(){
    digitalWrite(motorStandby, LOW);
    digitalWrite(motorInputOne, LOW);
    digitalWrite(motorInputTwo, LOW);
}