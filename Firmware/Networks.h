#ifndef NETWORKS_H
#define NETWORKS_H

#include <Arduino.h>
#include <WiFi.h>
#include "Display.h"
#include "Config.h"
#include <Arduino_JSON.h>
#define API_UPDATE_TIME 2000
#define HTTP_TIMEOUT 3000

// Declare pumpState variable
extern JSONVar pumpState;

// Declare fetchPump function
bool fetchPump();

// Declare jsonData variable
extern JSONVar jsonData;
// Fetch API data
bool fetchApi();
bool updateApi();


// Setting up Wifi
extern bool wifiIsConnected;
extern bool updateApiState;
// void WiFiEvent(WiFiEvent_t event);
void wifiEvent();
bool checkWifiState();
// void successWifi();
void connectWifi();
void initializeWifi();

#endif