#ifndef SENSORS_H
#define SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
// TODO: most remove this line please 💩 -> 5, remove it anywhere you see it
// #include <cstdint>

// For Soil Moisture Sensor
extern int soil_moisture_sens_one;
extern int soil_moisture_sens_two;

float SoilMoistureTranslator(int soilMoistureValue);
int readSoilMoisture(int specificSensor = 0);
int storeSoilMoisture();

// For Temperature and Humidity Sensor
extern bool BME0StartupState;
extern bool BME1StartupState;

bool initializeSensorOne();
bool initializeSensorTwo();
bool initializeSensorHelper(TwoWire *Mywire, Adafruit_BME280 _bme_);
void initializeSensor();
float readTemperature(int specificSensor = 1);
float readHumidity(int specificSensor = 1);
float readPressure(int specificSensor = 1);

#endif