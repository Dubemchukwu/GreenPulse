#include <Arduino.h>
#include "Networks.h"
#include "Sensors.h"
#include <HTTPClient.h>

JSONVar jsonData;
JSONVar pumpState;

bool updateApiState;

bool fetchPump(){
    if (!wifiIsConnected) {
        return false;
    }
    // Initialize the HTTPClient object
    HTTPClient client;
    
    // Connect to the API endpoint
    client.begin(API_ENDPOINT);
    
    // Set timeout for the HTTPClient object
    client.setTimeout(HTTP_TIMEOUT);
    
    // Add header to the HTTPClient object
    client.addHeader("User-Agent", "ESP32_Client/1.0");
    
    // Send the GET request
    int httpResponseCode = client.GET();
    
    if (httpResponseCode > 0) {
        if(httpResponseCode == HTTP_CODE_OK){
            String json = client.getString();
            pumpState = JSON.parse(json);
        }else{
            Serial.printf("[API-PUMP] Get Request Failed, error code: %s\r\n", client.errorToString(httpResponseCode).c_str());
            return false;
        }
    }else{
        Serial.printf("[API-PUMP] Connection failed, error code: %s\r\n", client.errorToString(httpResponseCode).c_str());
        return false;
    }
    
    client.end();
    return JSON.typeof(pumpState) != "undefined";
}

bool fetchApi(){
    if (!wifiIsConnected) {
        return false;
    }
    // Initialize the HTTPClient object
    HTTPClient client;
    
    // Connect to the API endpoint
    client.begin(API_ENDPOINT);
    
    // Set timeout for the HTTPClient object
    client.setTimeout(HTTP_TIMEOUT);
    
    // Add header to the HTTPClient object
    client.addHeader("User-Agent", "ESP32_Client/1.0");
    
    // Send the GET request
    int httpResponseCode = client.GET();
    
    if (httpResponseCode > 0) {
        if(httpResponseCode == HTTP_CODE_OK){
            String json = client.getString();
            
            // Parse JSON data
            jsonData = JSON.parse(json);
        }else{
            Serial.printf("[API-TEST] Get Request Failed, error code: %s\r\n", client.errorToString(httpResponseCode).c_str());
        }
    }else{
        Serial.printf("[API-TEST] Connection failed, error code: %s\r\n", client.errorToString(httpResponseCode).c_str());
    }
    
    client.end();
    Serial.printf("[JSON] Api Fetch Successfull\n");
    return JSON.typeof(pumpState) != "undefined";

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
    
    // Initialize the HTTPClient object
    HTTPClient client;
    
    // Connect to the API endpoint
    client.begin(API_ENDPOINT);
    
    // Set timeout for the HTTPClient object
    client.setTimeout(HTTP_TIMEOUT);
    
    // Add header to the HTTPClient object
    client.addHeader("Content-Type", "application/json");
    client.addHeader("User-Agent", "ESP32_Client/1.0");
    
    // Send the PUT request
    int httpResponseCode = client.PUT(jsonPayload);
    
    if (httpResponseCode > 0) {
        if(httpResponseCode == HTTP_CODE_OK || httpResponseCode == HTTP_CODE_CREATED || httpResponseCode == HTTP_CODE_NO_CONTENT){
            String json = client.getString();
        }else{
            Serial.printf("[API-UPDATE] Get Request Failed, error code: %s\r\n", client.errorToString(httpResponseCode).c_str());
            return false;
        }
    }else{
        Serial.printf("[API-UPDATE] Connection failed, error code: %s\r\n", client.errorToString(httpResponseCode).c_str());
        return false;
    }
    
    client.end();
    return true;
}