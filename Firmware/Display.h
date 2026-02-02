#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "Networks.h"

#define WIFI_DISPLAY_TIME 4000
enum CurrentScreen {HOME, WIFI};

extern bool OledDisplayInitState;
extern long lastWifiDisplayTime;
extern CurrentScreen currentScreen;
extern CurrentScreen previousScreen;

void initializeOledDisplay();
void setupOledDisplay();
void clearDisplayContent();
void showReadings(int soilMoistureValue, float tempValue, float humidityValue);
void displayWifiState();

#endif