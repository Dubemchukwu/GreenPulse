#include <Arduino.h>
#include <WiFi.h>
#include "Display.h"
#include "Networks.h"

#define MAX_RETRIES 7
#define MAX_RETRIES_DELAY 500

int retryCount = 0;
bool retryMessageState = false;
unsigned long lastCheckTime = 0;
bool wifiIsConnected = false;
bool previousState = false;

// WiFi event handler for ESP32-C6
// void WiFiEvent(WiFiEvent_t event) {
//     switch(event) {
//         case ARDUINO_EVENT_WIFI_STA_START:
//             Serial.println("[WiFi] Station Started");
//             break;
            
//         case ARDUINO_EVENT_WIFI_STA_CONNECTED:
//             Serial.println("[WiFi] Connected to AP");
//             wifiIsConnected = true;
//             break;
            
//         case ARDUINO_EVENT_WIFI_STA_GOT_IP:
//             Serial.println("[WiFi] Got IP Address");
//             Serial.printf("[WiFi] IP: %s\n", WiFi.localIP().toString().c_str());
//             wifiIsConnected = true;
//             retryMessageState = false;
//             break;
            
//         case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
//             Serial.println("[WiFi] Disconnected from AP");
//             wifiIsConnected = false;
//             break;
            
//         case ARDUINO_EVENT_WIFI_STA_LOST_IP:
//             Serial.println("[WiFi] Lost IP Address");
//             wifiIsConnected = false;
//             break;
            
//         default:
//             break;
//     }
// }

void wifiEvent(){
    wl_status_t status = WiFi.status();
    switch(status){
        case WL_CONNECTED:
            previousState = wifiIsConnected;
            wifiIsConnected = true;
            break;
        case WL_CONNECTION_LOST:
            previousState = wifiIsConnected;
            wifiIsConnected = false;
            break;
        case WL_DISCONNECTED:
            previousState = wifiIsConnected;
            wifiIsConnected = false;
            break;
    }
    
    if(previousState != wifiIsConnected){
        previousScreen = currentScreen;
        currentScreen = WIFI;
        lastWifiDisplayTime = millis()+1000;
    }
}

bool checkWifiState() {
    return wifiIsConnected && (WiFi.status() == WL_CONNECTED);
}

void connectWifi() {
    if (millis() - lastCheckTime >= MAX_RETRIES_DELAY) {
        if (retryCount >= MAX_RETRIES) {
            Serial.println("\n[WiFi] Maximum retries reached");
            retryCount = 0;
            vTaskDelay(500 / portTICK_PERIOD_MS);
            initializeWifi();
        }
        
        if (!retryMessageState) {
            Serial.print("[WiFi] Reconnecting .");
            retryMessageState = true;
        }
        
        Serial.print(".");
        WiFi.reconnect();
        retryCount++;
        lastCheckTime = millis();
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void initializeWifi() {
    Serial.println("[WiFi] Initializing...");
    
    // Register event handler BEFORE WiFi.begin()
    // WiFi.onEvent(WiFiEvent);
    
    // Disconnect and clear
    WiFi.disconnect(true);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    
    // Clear WiFi mode
    WiFi.mode(WIFI_OFF);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    
    // Configure WiFi
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("GreenPulse");
    
    // Start connection
    Serial.printf("[WiFi] Connecting to %s", SSID1);
    WiFi.begin(SSID1, PASSWORD1);
    
    // Wait for connection
    int timeout = MAX_RETRIES;
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
        vTaskDelay(700 / portTICK_PERIOD_MS);
        Serial.print(".");
        timeout--;
    }
    
    Serial.println();
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WiFi] Initial connection failed");
        wifiIsConnected = false;
    }else{
        wifiIsConnected = true;
        Serial.printf("[WiFi] WiFi Connected Successfully to %s\r\n", SSID1);
    }
}