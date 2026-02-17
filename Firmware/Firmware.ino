#include "Sensors.h"
#include "Display.h"
#include "Networks.h"
#include "Utils.h"
#include "Actuator.h"
#include <WiFi.h>
#include "Utils.h"

#define LED_BUILTIN 15
#define SOIL_MOISTURE_THRESHOLD 70
#define SOIL_DRY_VALUE 45
#define GAMMA 2.7182818285

long lastApiFetchTime = 0;
int wifiScreenTimeCounter = 0;
bool waterPumpState = false;

void setup() {
  Serial.begin(115200);
  
  // Initialize permanent storage
  initializePermanentStorage();
  
  // I2C setup
  // Wire1.begin(9, 8);
  Wire.begin(6, 7);
  // Wire.setClock(50000);
  // Wire1.setClock(50000);
  
  xTaskCreatePinnedToCore(
      initManager,
      "InitManager",
      2048,
      NULL,
      1,
      NULL,
      0
  );
  
  // WiFi Setup
  initializeWifi();
  
  // Initialize web server
  initializeWebServer();
  
  // Initialize and Setup Actuator
  initializeActuator();
  
  // GPIO setup
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("[Setup] Doing something Here");
  
  xTaskCreatePinnedToCore(
      displayManager,
      "DisplayManager",
      6144,
      NULL,
      3,
      NULL,
      0
  );
  

  xTaskCreatePinnedToCore(
    logicManager,
    "LogicManager",
    2048,
    NULL,
    3,
    NULL,
    0
  );

  xTaskCreatePinnedToCore(
    wifiManager,
    "WiFiManager",
    4096,
    NULL,
    3,
    NULL,
    0
  );
  
  xTaskCreatePinnedToCore(
    irrigationManager,
    "IrrigationManager",
    2048,
    NULL,
    1,
    NULL,
    0
  );

  xTaskCreatePinnedToCore(
    utilsManager,
    "utilsManager",
    2048,
    NULL,
    1,
    NULL,
    0
  );
  
}

// Initialize I2C stuffs in different task to speed up boot time
void initManager(void *pvParameters){
    // Initialize the system
    
    // Initialize and Setup Sensor
    initializeSensor();
    
    // Initialize and Setup OLED display
    initializeOledDisplay();
    setupOledDisplay();
    
    vTaskDelete(NULL);
}

// Automatic Irrigation Implementation
void irrigationManager(void *pvParameters){
    for(;;){
        
        // Upper average soil moisture
        int HIGH_AVERAGE_SOIL_MOISTURE = SOIL_DRY_VALUE + (SOIL_MOISTURE_THRESHOLD - SOIL_DRY_VALUE)/2;
        // Lower average soil moisture
        int LOW_AVERAGE_SOIL_MOISTURE = SOIL_DRY_VALUE + (HIGH_AVERAGE_SOIL_MOISTURE - SOIL_DRY_VALUE)/2 - 1;
        
        if(SoilMoistureTranslator(readSoilMoisture()) <= LOW_AVERAGE_SOIL_MOISTURE){
            waterPumpState = true;
        }else if(SoilMoistureTranslator(readSoilMoisture()) >= HIGH_AVERAGE_SOIL_MOISTURE){
            waterPumpState = false;
        }
        
        if(waterPumpState){
            float normalizeControlValue = (float)(HIGH_AVERAGE_SOIL_MOISTURE - SoilMoistureTranslator(readSoilMoisture()))/(float)(HIGH_AVERAGE_SOIL_MOISTURE - LOW_AVERAGE_SOIL_MOISTURE);
            int exponentialControlValue = constrain(100*(pow(normalizeControlValue, GAMMA)), 0, 100);
            moveActuatorPrecise(exponentialControlValue, 1);
            // Serial.printf("[IRG] Watering Plants, MOS: %d\r\n", SoilMoistureTranslator(readSoilMoisture()));
        }else{
            stopActuator();
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void wifiManager(void *pvParameters){
    for(;;){
        wifiEvent();
        if(!wifiIsConnected){
            connectWifi();
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// Manages the Programs logic to automatically control irrigation
void logicManager(void *pvParameters) {
  for(;;){
    // Logic Manager
    bool fetchPumpState = fetchPump();
    if(fetchPumpState){
        if(SoilMoistureTranslator(readSoilMoisture()) < SOIL_MOISTURE_THRESHOLD){
            // Implement Exponential Control logic curve
            // Note: Future Dubem 💀😏 to Implement it
            // Already did it, thanks past Dubem
            
            digitalWrite(LED_BUILTIN, ((String)pumpState["pump"] == "on") ? HIGH : LOW);
            // int proportionalControlValue = map(SoilMoistureTranslator(readSoilMoisture()), SOIL_DRY_VALUE, SOIL_MOISTURE_THRESHOLD, 0, 100);
        
            // Implemented Exponential Control logic curve for Watering / Irrigation
            // Created for pump button control to avoid overwatering and prevent pump from running dry
            if((String)pumpState["pump"] == "on"){
                float normalizeControlValue = (float)(SOIL_MOISTURE_THRESHOLD - SoilMoistureTranslator(readSoilMoisture()))/(float)(SOIL_MOISTURE_THRESHOLD - SOIL_DRY_VALUE);
                int exponentialControlValue = constrain(100*(pow(normalizeControlValue, GAMMA)), 0, 100);
                // proportionalControlValue = constrain(proportionalControlValue, 0, 100);
                // Serial.printf("[LOGIC] Porportional Control Value: %d\n", proportionalControlValue);
                Serial.printf("[LOGIC] Exponential Control Value: %d\n", exponentialControlValue);
                moveActuatorPrecise(exponentialControlValue, 1);
                // moveActuator();
            }else{
                stopActuator();
            }
        }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// Manages and Sends logs to their various destinations
void utilsManager(void *pvParameters){
  for(;;){
      server.handleClient();
      if(restartESPWEB){
          if(millis() - lastESPWEBTime > ESP_WEB_RESTART_DELAY){
              ESP.restart();
          }
      }
      // loggerCallback(serialLogger);
      yield();
      vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// Manages and Chooses what should be displayed on the OLED display
void displayManager(void *pvParameters) {
  for(;;){
    switch (currentScreen) {
      case WIFI:
        if(millis() - lastWifiDisplayTime <= WIFI_DISPLAY_TIME){
            // Serial.println("[DISPLAY-MANAGER] Displaying Wifi State");
            wifiScreenTimeCounter++;
            Serial.printf("[DISPLAY-MANAGER] Wifi Screen Time Counter: %d\r\n", (millis() - lastWifiDisplayTime));
            displayWifiState();
            rgbLedWrite(8, wifiIsConnected ? 0 : 255, wifiIsConnected ? 255 : 0, 0);
            // digitalWrite(15, wifiIsConnected ? HIGH : LOW);
        }else{
            previousScreen = currentScreen;
                currentScreen = HOME;
        }
        break;
      case HOME:
        wifiScreenTimeCounter = 0;
        rgbLedWrite(8, 0, 0, 0);
        showReadings(SoilMoistureTranslator(readSoilMoisture(0)), readTemperature(1), readHumidity(1));
        break;
    }
    // Displaying Sensor Data
    // Serial.println("[DISPLAY-MANAGER] Hi, 😒");
    // Serial.println(SoilMoistureTranslator(readSoilMoisture(0)));
    // Serial.println(readTemperature(1));
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void loop() {
 // Working Something here
 // rgbLedWrite(8, 255, 255, 255);


 if(millis() - lastApiFetchTime >= API_UPDATE_TIME){
     updateApiState = updateApi();
     // Serial.printf("[API] Success State %s\r\n", updateApiState ? "TRUE" : "FALSE");
     // if(updateApiState){
     //     rgbLedWrite(8, 0, 255, 0);
     // }else{
     //     rgbLedWrite(8, 255, 0, 0);
     // };
     
     yield();
     lastApiFetchTime = millis();
 }else{
     vTaskDelay(pdMS_TO_TICKS(API_UPDATE_TIME+50));
 };
 
 
}
