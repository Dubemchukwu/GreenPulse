#include <Arduino.h>
#include "Networks.h"
#include <WiFiClientSecure.h>

JSONVar jsonData;

bool fetchApi(){
    if (!wifiIsConnected) {
        return false;
    }
    // Initialize the WiFiClientSecure object
    WiFiClientSecure *client = new WiFiClientSecure;
    
    // Check if the client was successfully created
    if (!client) {
        return false;
    }
    
    // Set the client to use insecure mode
    client->setInsecure();
    
    // Connect to the API endpoint
    client->connect(API_ENDPOINT, PORT);
    if (!client->connected()) {
        delete client;
        return false;
    }
    
    // Get the response
    String response = String("GET /sensor/api HTTP/1.1\r\nHost: ") + API_ENDPOINT + "\r\n" + "User-Agent: ESP32\r\n\r\n";
    
    response += "Connection: close\r\n\r\n";
    client->print(response);
    
    // Wait for the response
    unsigned long timeout = millis();
      while (!client->available()) {
        if (millis() - timeout > HTTP_TIMEOUT) {
          client->stop();
          delete client;
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
    // Serial.println(json);
    } else {
    Serial.println("JSON not found in response");
    }
    
    client->stop();
    delete client;
    
    if (statusCode != 200){
        Serial.printf("[HTTPS] ❌ Status: %d\n", statusCode);
        return false;
    }
    // Parse JSON data
    jsonData = JSON.parse(json);
    return JSON.typeof(jsonData) != "undefined";
}