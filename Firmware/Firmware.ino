#include "Sensors.h"
#include "Display.h"
#include <WiFi.h>
#include <Adafruit_BME280.h>

#define LED_BUILTIN 15

void setup() {
  Serial.begin(115200);
  
  // WiFi Setup
  WiFi.begin();
  
  // I2C setup
  Wire1.begin(8, 9);
  Wire.begin(6, 7);
  Wire.setClock(50000);
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
    logicManager,
    "LogicManager",
    6144,
    NULL,
    3,
    NULL,
    0
  );
}

// Manages the Programs logic to automatically control irrigation
void logicManager(void *pvParameters) {
  for(;;){
    // Logic Manager
    Serial.println("[LOGIC-MANAGER] Hi, 🤨");
    // Serial.println(readSoilMoisture(0));
    // Serial.println(readTemperature(1));
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// Manages and Chooses what should be displayed on the OLED display
void displayManager(void *pvParameters) {
  for(;;){
    // Displaying Sensor Data
    Serial.println("[DISPLAY-MANAGER] Hi, 😒");
    Serial.println(readSoilMoisture(0));
    Serial.println(readTemperature(1));
    showReadings(readSoilMoisture(0), readTemperature(1), readHumidity(1));
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void loop() {
 // Working Something here
 digitalWrite(LED_BUILTIN, HIGH);
 Serial.println(readSoilMoisture());
 vTaskDelay(500 / portTICK_PERIOD_MS);
 digitalWrite(LED_BUILTIN, LOW);
 vTaskDelay(500 / portTICK_PERIOD_MS);
}
