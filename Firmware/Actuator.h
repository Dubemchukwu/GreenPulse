#ifndef ACTUATOR_H
#define ACTUATOR_H

void initializeActuator();
void stopActuator();
void moveActuator();
void moveActuatorPrecise(int speed = 50, int direction = 1);
void reverseActuator();
bool getPumpState();

#endif