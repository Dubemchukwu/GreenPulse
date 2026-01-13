#include "Utils.h"
#include "Sensors.h"

long logsPastTime = millis();

void serialLogger(){
    // It will be logged in this order temperature, humidity, pressure, soil moisture
    float temperature = readTemperature();
    float humidity = readHumidity();
    float pressure = readPressure();
    float soilMoisture = SoilMoistureTranslator(readSoilMoisture());
    Serial.printf("[Logs] %.2f, %.2f, %.2f, %.2f\n", temperature, humidity, pressure/100.0F, soilMoisture);
}

void loggerCallback(void (*logsFunc)(), int _delay_){
    if(millis() - logsPastTime >= _delay_){
        logsFunc();
        logsPastTime = millis();
    }
}