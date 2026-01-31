#include <Arduino.h>
#include "Networks.h"
#include "Sensors.h"
#include "Actuator.h"
#include <NetworkClient.h>

JSONVar jsonData;
JSONVar pumpState;

bool updateApiState;

bool fetchPump(){
    if (!wifiIsConnected) {
        return false;
    }
    // Initialize the WiFiClientSecure object
    NetworkClient *client = new NetworkClient;
    
    // Check if the client was successfully created
    if (!client) {
        return false;
    }
    
    // Connect to the API endpoint
    while (!client->connect("dubemguy.pythonanywhere.com", 80)) {
        // delete client;
        Serial.println("[API] Connection failed");
    }
    
    // Get the response
    client->println("GET /sensor/api HTTP/1.1");
    client->println("Host: dubemguy.pythonanywhere.com");
    client->println("User-Agent: ESP32_Client/1.0");
    client->println("Connection: close");
    client->println(); 
    
    // Serial.println("[API] Request sent. Waiting for response...");
    
    // Wait for the response
    unsigned long timeout = millis();
      while (!client->available()) {
        if (millis() - timeout > HTTP_TIMEOUT) {
          client->stop();
          delete client;
          Serial.println("[API] Connection timed out");
          return false;
        }
      }
    
    // Parse response
    String body = "";
    bool inBody = false;
    int statusCode = 0;
    
    while (client->available()) {
        String line = client->readStringUntil('\n');
        
        if (statusCode == 0 && line.startsWith("HTTP/")) {
        statusCode = line.substring(9, 12).toInt();
        }
        
        if (line == "\r" || line.isEmpty()) {
        inBody = true;
        continue;
        }
        
        if (inBody) body += line;
    }
    
    int jsonStart = body.indexOf('{');          
    int jsonEnd = body.lastIndexOf('}');
    String json;

    if (jsonStart != -1 && jsonEnd != -1) {
    json = body.substring(jsonStart, jsonEnd + 1);
    } else {
    Serial.println("[API] JSON not found in response");
    }
    
    client->stop();
    delete client;
    
    if (statusCode != 200){
        Serial.printf("[HTTPS] ❌ Status: %d\n", statusCode);
        return false;
    }
    // Parse JSON data
    pumpState = JSON.parse(json);
    // Serial.printf("[JSON] Api Fetch Successfull\n");
    return JSON.typeof(pumpState) != "undefined";
}

bool fetchApi(){
    if (!wifiIsConnected) {
        return false;
    }
    // Initialize the WiFiClientSecure object
    NetworkClient *client = new NetworkClient;
    
    // Check if the client was successfully created
    if (!client) {
        return false;
    }
    
    // Connect to the API endpoint
    while (!client->connect("dubemguy.pythonanywhere.com", 80)) {
        // delete client;
        Serial.println("[API] Connection failed");
    }
    
    // Get the response
    client->println("GET /sensor/api HTTP/1.1");
    client->println("Host: dubemguy.pythonanywhere.com");
    client->println("User-Agent: ESP32_Client/1.0");
    client->println("Connection: close");
    client->println(); 
    
    Serial.println("[API] Request sent. Waiting for response...");
    
    // Wait for the response
    unsigned long timeout = millis();
      while (!client->available()) {
        if (millis() - timeout > HTTP_TIMEOUT) {
          client->stop();
          delete client;
          Serial.println("[API] Connection timed out");
          return false;
        }
      }
    
    // Parse response
    String body = "";
    bool inBody = false;
    int statusCode = 0;
    
    while (client->available()) {
        String line = client->readStringUntil('\n');
        
        if (statusCode == 0 && line.startsWith("HTTP/")) {
        statusCode = line.substring(9, 12).toInt();
        }
        
        if (line == "\r" || line.isEmpty()) {
        inBody = true;
        continue;
        }
        
        if (inBody) body += line;
    }
    
    int jsonStart = body.indexOf('{');          
    int jsonEnd = body.lastIndexOf('}');
    String json;

    if (jsonStart != -1 && jsonEnd != -1) {
    json = body.substring(jsonStart, jsonEnd + 1);
    Serial.println(json);
    } else {
    Serial.println("[API] JSON not found in response");
    }
    
    client->stop();
    delete client;
    
    if (statusCode != 200){
        Serial.printf("[HTTPS] ❌ Status: %d\n", statusCode);
        return false;
    }
    // Parse JSON data
    jsonData = JSON.parse(json);
    Serial.printf("[JSON] Api Fetch Successfull\n");
    return JSON.typeof(jsonData) != "undefined";
}

String parseJson() {
    JSONVar jsonData;
    jsonData["temperature"] = readTemperature();
    jsonData["humidity"] = readHumidity();
    jsonData["pressure"] = readPressure();
    jsonData["soil_moisture"] = SoilMoistureTranslator(readSoilMoisture());
    return JSON.stringify(jsonData);
}

bool updateApi(){
    String jsonPayload = parseJson();
    if (!wifiIsConnected) {
        return false;
    }
    // Initialize the WiFiClientSecure object
    NetworkClient *client = new NetworkClient;
    
    // Check if the client was successfully created
    if (!client) {
        return false;
    }
    
    // Connect to the API endpoint
    while (!client->connect(API_ENDPOINT, PORT)) {
        // delete client;
        Serial.println("[API] Connection failed");
        return false;
    }
    
    client->println("PUT /sensor/api HTTP/1.1");
    client->println("Host: dubemguy.pythonanywhere.com");
    client->println("User-Agent: ESP32_Client/1.0");
    client->println("Content-Type: application/json"); // Important for JSON
    client->print("Content-Length: ");
    client->println(jsonPayload.length());
    
    client->println("Connection: close");
    client->println();
    client->print(jsonPayload);

    // Serial.println("[API] Request sent. Waiting for response...");
    
    unsigned long timeout = millis();
      while (client->available() == 0) {
        if (millis() - timeout > 5000) {
          Serial.println("[API] Client Timeout: Server didn't respond in 5s");
          client->stop();
          return false;
        }
      }
    
      String statusLine = client->readStringUntil('\n');
      // Serial.print("[API] Server Response Status: ");
      // Serial.println(statusLine);
      // Serial.printf("\n");    
    
      client->stop();
      return true;
}