#include "Actuator.h"
#include <Arduino.h>

uint8_t motorInputOne = 0x13;
uint8_t motorInputTwo = 0x14;
uint8_t motorStandby = 0x12;

#define LEDC_FREQ 8000
#define LEDC_RES 8
#define MAX_SPEED 255

void initializeActuator() {
    // Initialize Actuator(Pump) and Set it to Stop, to avoid misharps
    pinMode(motorStandby, OUTPUT);
    
    // Initialize LEDC library for motor control
    ledcAttach(motorInputOne, LEDC_FREQ, LEDC_RES);
    ledcAttach(motorInputTwo, LEDC_FREQ, LEDC_RES);
    stopActuator();
}

void moveActuatorPrecise(int speed, int direction){
    // converting speed to duty cycle
    int dutyCycle = map(speed, 0, 100, 0, (MAX_SPEED+1));
    
    // Pump Water in forward direction that's into the soil
    if(direction == 1){
        digitalWrite(motorStandby, HIGH);
        ledcWrite(motorInputOne, dutyCycle);
        ledcWrite(motorInputTwo, 0);
    }
    // Pump Water in reverse direction that's away from the soil to the reservoir
    else if(direction == -1){
        digitalWrite(motorStandby, HIGH);
        ledcWrite(motorInputTwo, dutyCycle);
        ledcWrite(motorInputOne, 0);
    }
}

void moveActuator(){
    // Pump Water in forward direction that's into the soil
    digitalWrite(motorStandby, HIGH);
    ledcWrite(motorInputOne, MAX_SPEED);
    ledcWrite(motorInputTwo, 0);
}

void reverseActuator(){
    // Pump water in reverse direction that's away from the soil to the reservoir
    digitalWrite(motorStandby, HIGH);
    ledcWrite(motorInputOne, 0);
    ledcWrite(motorInputTwo, MAX_SPEED);
}

void stopActuator(){
    // Stop motor pumping actions
    digitalWrite(motorStandby, LOW);
    digitalWrite(motorInputOne, 0);
    digitalWrite(motorInputTwo, 0);
}

bool getPumpState(){
    // Get Pump state 
    return (bool)ledcRead(motorStandby) && ((bool)ledcRead(motorInputOne) || (bool)ledcRead(motorInputTwo));
}