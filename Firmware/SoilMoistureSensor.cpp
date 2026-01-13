#include "Sensors.h"
#include <Arduino.h>

int soil_moisture_sens_one = 0;
int soil_moisture_sens_two = 1;

 float SoilMoistureTranslator(int soilMoistureValue){
    return (float)(soilMoistureValue*100)/4095.0;
}
/* 
 * There are only two sensor Available here so, for the parameters
 * input 0 for combined readings and 1 or 2 for sensor individual readings.
 * Anything other than that would simply take you to default.
 */
int readSoilMoisture(int specificSensor){
    if(specificSensor == 0){
        int soil_moisture_value = (int)((analogRead(soil_moisture_sens_one) + analogRead(soil_moisture_sens_two))/2);
        return soil_moisture_value;
    }else if(specificSensor == 1){
        int soil_moisture_value = analogRead(soil_moisture_sens_one);
        return soil_moisture_value;      
    }else if(specificSensor == 2){
        int soil_moisture_value = analogRead(soil_moisture_sens_two);
        return soil_moisture_value;    
    }else{
        int soil_moisture_value = (analogRead(soil_moisture_sens_one) + analogRead(soil_moisture_sens_two))/2;
        return soil_moisture_value;
    }
}

int storeSoilMoisture(){
    int gottenSoilMoistureValue = readSoilMoisture(0);
    return gottenSoilMoistureValue;
}