#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
// for now i will focus on serial logging instead of other efficient forms
extern long logsPastTime;
void serialLogger();
void loggerCallback(void (*logsFunc)(), int _delay_ = 1000);

#endif