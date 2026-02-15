#ifndef ACTUATOR_H
#define ACTUATOR_H

// Initialize LEDC library for motor control
void initializeActuator();

// Actuator movements command
void stopActuator();
void moveActuator();
void moveActuatorPrecise(int speed = 50, int direction = 1);
void reverseActuator();

// Fetch pump state
bool getPumpState();

#endif