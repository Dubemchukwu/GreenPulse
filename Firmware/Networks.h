#ifndef NETWORKS_H
#define NETWORKS_H

#include <Arduino.h>
#include <WiFi.h>
#include "Display.h"
#include "Config.h"
#include <Arduino_JSON.h>
#define API_TIMEOUT 2000

extern JSONVar jsonData;
// Fetch API data
bool fetchApi();
// Setting up Wifi
extern bool wifiIsConnected;
// void WiFiEvent(WiFiEvent_t event);
void wifiEvent();
bool checkWifiState();
// void successWifi();
void connectWifi();
void initializeWifi();

#endif