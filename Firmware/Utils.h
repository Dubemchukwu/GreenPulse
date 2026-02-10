#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <stdint.h>
// for now i will focus on serial logging instead of other efficient forms
extern long logsPastTime;
void serialLogger();
void loggerCallback(void (*logsFunc)(), int _delay_ = 1000);

// Wifi Credentials
extern String ssid;
extern String password;

// Initialize web server
extern WebServer server;
void initializeWebServer();

// Initialize permanent storage
void initializePermanentStorage();

#endif