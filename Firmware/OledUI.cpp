#include "Display.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ADDRESS 0x3C
#define RESET -1
#define WIDTH 128
#define HEIGHT 64
#define CHAR_WIDTH 6
#define CHAR_HEIGHT 20

bool OledDisplayInitState;

Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, RESET);

void initializeOledDisplay(){
    Serial.printf("[Display] Initializing SSD1306 0.96' OLED Display ");
    OledDisplayInitState = display.begin(0x02, 0x3C);
    for(int i=0; i<=5; i++){
        if(!OledDisplayInitState){
            OledDisplayInitState = display.begin(0x02, 0x3C);
            vTaskDelay(pdMS_TO_TICKS(250));
            Serial.print(".");
        }else{
            break;
        }
    }
    
    Serial.printf("\n");
    if(!OledDisplayInitState){
        Serial.printf("[Display] Initialization of SSD1306 OLED has failed due to some forseen reasons, bro");
        Serial.printf("[Display] Check your wiring, it's literally labelled on the module, 🙄");
        return;
    }else if(OledDisplayInitState){
        Serial.println("[Display] SuccessFully Initialized SSD1306 OLED 0.96' 🫠");
    }else{
        // Do nothing, if possible. This is probably not needed but will come back to it later.
    }
    Serial.println("");
}

void setupOledDisplay(){
    if(OledDisplayInitState){
        display.clearDisplay();
        display.setCursor(0,0);
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.cp437(true);
        display.drawRoundRect(0,0,WIDTH-2,HEIGHT-2,2,SSD1306_WHITE);
        display.drawRoundRect(1,1,WIDTH-3,HEIGHT-3,2,SSD1306_WHITE);
        display.display();
    }
}

void clearDisplayContent(){
    display.clearDisplay();
    display.drawRoundRect(0,0,WIDTH-2,HEIGHT-2,2,SSD1306_WHITE);
    display.drawRoundRect(1,1,WIDTH-3,HEIGHT-3,2,SSD1306_WHITE);
    display.display();
}

void showReadings(int soilMoistureValue, float tempValue, float humidityValue){
    if(!OledDisplayInitState){
        return;
    }
    // Ensure that the screen is in proper format
    clearDisplayContent();
    
    // First Line of Text
    display.setCursor(10,8);
    display.print("SOIL: ");
    display.setCursor((6*CHAR_WIDTH)+10,8);
    display.printf("%d", soilMoistureValue);
    display.setCursor((6*CHAR_WIDTH)+22,8);
    display.print("%");
    
    // Second Line of Text 
    display.setCursor(10, CHAR_HEIGHT+8);
    display.print("TEMP: ");
    display.setCursor((6*CHAR_WIDTH)+10, CHAR_HEIGHT+8);
    display.printf("%.2f C", tempValue);
    
    // Third Line of Text
    display.setCursor(10, (2*CHAR_HEIGHT)+8);
    display.print("HUMD: ");
    display.setCursor((6*CHAR_WIDTH)+10, (2*CHAR_HEIGHT)+8);
    display.printf("%.2f ", humidityValue);
    display.setCursor((10*CHAR_WIDTH)+22, (2*CHAR_HEIGHT)+8);
    display.print("%");
    
    display.display();
    
}
