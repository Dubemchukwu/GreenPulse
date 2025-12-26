#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

extern bool OledDisplayInitState;

void initializeOledDisplay();
void setupOledDisplay();
void clearDisplayContent();
void showReadings(int soilMoistureValue, float tempValue, float humidityValue);

#endif