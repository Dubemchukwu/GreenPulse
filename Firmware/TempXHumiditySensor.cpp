#include "Sensors.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

bool BME0StartupState;
bool BME1StartupState;
uint8_t BMENum = 0;

Adafruit_BME280 bme0;
Adafruit_BME280 bme1;

// Deprecated ... 🥲, became too stressful to use 🛠️
bool initializeSensorHelper(TwoWire *Mywire, Adafruit_BME280 _bme_){
    // bool startupState;
    Serial.printf("[Sensor] Initializing BME280 temperature and Humidity Sensor %d ", BMENum);
    Serial.println(_bme_.begin(0x76, Mywire));
    static unsigned startupState = _bme_.begin(0x76, Mywire);
    Serial.println(startupState);
    
    for(int i=0; i<=5; i++){
        if(!startupState){
            startupState = _bme_.begin(0x76, Mywire);
            vTaskDelay(pdMS_TO_TICKS(250));
            Serial.print(".");
        }else{
            break;
        }
    }
    
    Serial.println("");
    if(!startupState){
        Serial.printf("[Sensor] Initialization of BME280 has Failed Woofly, think about your life! 😏\n");
        Serial.println("[Sensor] Check your wiring man, Gosh!");
        return startupState;
    }
    
    Serial.printf("[Sensor] SuccessFully Initialized BME280 Sensor %d 😒\n", BMENum);    
    BMENum++;
    Serial.println("");
    return startupState;
}

// Initializing I2C sensors individually, to avoid the hassle of managing pointers and references.
bool initializeSensorOne(){
    Serial.printf("[Sensor] Initializing BME280 temperature and Humidity Sensor 1 ");
    Serial.println(bme0.begin(0x76, &Wire));
    static unsigned startupState = bme0.begin(0x76, &Wire);
    Serial.println(startupState);
    
    for(int i=0; i<=5; i++){
        if(!startupState){
            startupState = bme0.begin(0x76, &Wire);
            vTaskDelay(pdMS_TO_TICKS(250));
            Serial.print(".");
        }else{
            break;
        }
    }
    
    Serial.println("");
    if(!startupState){
        Serial.printf("[Sensor] Initialization of BME280 has Failed Woofly, think about your life! 😏\n");
        Serial.println("[Sensor] Check your wiring man, Gosh!");
        return startupState;
    }
    
    Serial.printf("[Sensor] SuccessFully Initialized BME280 Sensor 1 😒\n");    
    Serial.println("");
    return startupState;
}

bool initializeSensorTwo(){
    Serial.printf("[Sensor] Initializing BME280 temperature and Humidity Sensor 2 ");
    Serial.println(bme1.begin(0x76, &Wire1));
    unsigned startupState;
    startupState = bme1.begin(0x76, &Wire1);
    Serial.println(startupState);
    
    for(int i=0; i<=5; i++){
        if(!startupState){
            startupState = bme1.begin(0x76, &Wire1);
            vTaskDelay(pdMS_TO_TICKS(250));
            Serial.print(".");
        }else{
            break;
        }
    }
    
    Serial.println("");
    if(!startupState){
        Serial.printf("[Sensor] Initialization of BME280 has Failed Woofly, think about your life! 😏\n");
        Serial.println("[Sensor] Check your wiring man, Gosh!");
        return startupState;
    }
    
    Serial.printf("[Sensor] SuccessFully Initialized BME280 Sensor 2 😒\n");    
    Serial.println("");
    return startupState;
}

// Initializing All BME280 sensors and quick test ✅
void initializeSensor(){
    BME0StartupState = initializeSensorOne();
    // BME1StartupState = initializeSensorTwo();
    Serial.println(bme0.readTemperature());
    Serial.println(bme0.readHumidity());
}

/* 
 * There are only two sensor Available here so, for the parameters
 * input 0 for combined readings and 1 or 2 for sensor individual readings.
 * Anything other than that would simply take you to default.
 */
float readTemperature(int specificSensor){
    if(specificSensor == 0){
        if(BME0StartupState == true && BME1StartupState == true){
            float tempReadings = (bme0.readTemperature() + bme1.readTemperature())/2;
            return tempReadings;
        }else{
            Serial.println("[Sensor] BME280 is not properly initialized; check it though 🫡!");
            Serial.println("[Sensor] I think it's Wire0, not so sure. Fairwell");            
            return NULL;
        }
    }else if(specificSensor == 1){
        if(BME0StartupState){
            return bme0.readTemperature();
        }else{
            Serial.println("[Sensor] BME280 is not properly initialized; check it though💀!");
            Serial.println("[Sensor] Surely it's Wire0, This one usually has no issues");            
            return NULL;
        }
    }else if(specificSensor == 2){
        if(BME1StartupState){
            return bme1.readTemperature();
        }else{
            Serial.println("[Sensor] BME280 is not properly initialized; Hmmm 🫡!");
            Serial.println("[Sensor] I am certain it's Wire1. Goodluck");            
            return NULL;
        }
    }else{
        if(BME0StartupState == true && BME1StartupState == true){
            float tempReadings = (bme0.readTemperature() + bme1.readTemperature())/2;
            return tempReadings;
        }else{
            Serial.println("[Sensor] BME280 is not properly initialized; check it though 🫡!");
            Serial.println("[Sensor] I think it's Wire0, not so sure. Fairwell");            
            return NULL;
        }
    }
}

/* 
 * There are only two sensor Available here so, for the parameters
 * input 0 for combined readings and 1 or 2 for sensor individual readings.
 * Anything other than that would simply take you to default.
 */
float readHumidity(int specificSensor){
    if(specificSensor == 0){
        if(BME0StartupState == true && BME1StartupState == true){
            float tempReadings = (bme0.readHumidity() + bme1.readHumidity())/2;
            return tempReadings;
        }else{
            Serial.println("[Sensor] BME280 is not properly initialized; check it though 🫡!");
            Serial.println("[Sensor] I think it's Wire0, not so sure. Fairwell");            
            return NULL;
        }
    }else if(specificSensor == 1){
        if(BME0StartupState){
            return bme0.readHumidity();
        }else{
            Serial.println("[Sensor] BME280 is not properly initialized; check it though💀!");
            Serial.println("[Sensor] Surely it's Wire0, This one usually has no issues");            
            return NULL;
        }
    }else if(specificSensor == 2){
        if(BME1StartupState){
            return bme1.readHumidity();
        }else{
            Serial.println("[Sensor] BME280 is not properly initialized; Hmmm 🫡!");
            Serial.println("[Sensor] I am certain it's Wire1. Goodluck");            
            return NULL;
        }
    }else{
        if(BME0StartupState == true && BME1StartupState == true){
            float tempReadings = (bme0.readHumidity() + bme1.readHumidity())/2;
            return tempReadings;
        }else{
            Serial.println("[Sensor] BME280 is not properly initialized; check it though 🫡!");
            Serial.println("[Sensor] I think it's Wire0, not so sure. Fairwell");            
            return NULL;
        }
    }
}

// This is fucntions is an extra and probably will never be used in my project 😎, but you never know what/
float readPressure(int specificSensor){
    if(specificSensor == 0){
        if(BME0StartupState == true && BME1StartupState == true){
            float tempReadings = (bme0.readPressure() + bme1.readPressure())/2;
            return tempReadings;
        }else{
            Serial.println("[Sensor] BME280 is not properly initialized; check it though 🫡!");
            Serial.println("[Sensor] I think it's Wire0, not so sure. Fairwell");            
            return NULL;
        }
    }else if(specificSensor == 1){
        if(BME0StartupState){
            return bme0.readPressure();
        }else{
            Serial.println("[Sensor] BME280 is not properly initialized; check it though💀!");
            Serial.println("[Sensor] Surely it's Wire0, This one usually has no issues");            
            return NULL;
        }
    }else if(specificSensor == 2){
        if(BME1StartupState){
            return bme1.readPressure();
        }else{
            Serial.println("[Sensor] BME280 is not properly initialized; Hmmm 🫡!");
            Serial.println("[Sensor] I am certain it's Wire1. Goodluck");            
            return NULL;
        }
    }else{
        if(BME0StartupState == true && BME1StartupState == true){
            float tempReadings = (bme0.readPressure() + bme1.readPressure())/2;
            return tempReadings;
        }else{
            Serial.println("[Sensor] BME280 is not properly initialized; check it though 🫡!");
            Serial.println("[Sensor] I think it's Wire0, not so sure. Fairwell");            
            return NULL;
        }
    }
}
