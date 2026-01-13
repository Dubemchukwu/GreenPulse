#include "Sensors.h"
#include "Display.h"
#include "Networks.h"
#include "Utils.h"
#include <WiFi.h>
#include <Adafruit_BME280.h>

#define LED_BUILTIN 15

long lastApiFetchTime = 0;

void setup() {
  Serial.begin(115200);
  
  // WiFi Setup
  initializeWifi();
  
  // I2C setup
  Wire1.begin(8, 9);
  Wire.begin(6, 7);
  // Wire.setClock(50000);
  // Wire1.setClock(50000);
  
  // GPIO setup
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Initialize and Setup OLED display
  initializeOledDisplay();
  setupOledDisplay();
  
  // Initialize and Setup Sensor
  initializeSensor();

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
    logsManager,
    "logsManager",
    4096,
    NULL,
    3,
    NULL,
    0
  );

  // xTaskCreatePinnedToCore(
  //   logicManager,
  //   "LogicManager",
  //   2048,
  //   NULL,
  //   3,
  //   NULL,
  //   0
  // );

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
    apiManager,
    "APIManager",
    4096,
    NULL,
    3,
    NULL,
    0
  );
}

void apiManager(void *pvParameters){
    for(;;){
        if(millis() - lastApiFetchTime > API_TIMEOUT){
            lastApiFetchTime = millis();
        }
    }
}

void wifiManager(void *pvParameters){
    for(;;){
        wifiEvent();
        if(!wifiIsConnected){
            connectWifi();
        }
        yield();
    }
}

// Manages the Programs logic to automatically control irrigation
void logicManager(void *pvParameters) {
  for(;;){
    // Logic Manager
    // Serial.println("[LOGIC-MANAGER] Hi, 🤨");
    // Serial.println(readSoilMoisture(0));
    // Serial.println(readTemperature(1));
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// Manages and Sends logs to their various destinations
void logsManager(void *pvParameters){
  for(;;){
    loggerCallback(serialLogger);
    yield();
  }
}

// Manages and Chooses what should be displayed on the OLED display
void displayManager(void *pvParameters) {
  for(;;){
    switch (currentScreen) {
      case WIFI:
        displayWifiState();
        digitalWrite(15, wifiIsConnected ? HIGH : LOW);
        previousScreen = currentScreen;
        currentScreen = HOME;
        break;
      case HOME:
        showReadings(SoilMoistureTranslator(readSoilMoisture(0)), readTemperature(1), readHumidity(1));
        break;
    }
    // Displaying Sensor Data
    // Serial.println("[DISPLAY-MANAGER] Hi, 😒");
    // Serial.println(SoilMoistureTranslator(readSoilMoisture(0)));
    // Serial.println(readTemperature(1));
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void loop() {
 // Working Something here
 // digitalWrite(LED_BUILTIN, HIGH);
 Serial.println(readSoilMoisture());
 vTaskDelay(500 / portTICK_PERIOD_MS);
 // digitalWrite(LED_BUILTIN, LOW);
 vTaskDelay(500 / portTICK_PERIOD_MS);
}
