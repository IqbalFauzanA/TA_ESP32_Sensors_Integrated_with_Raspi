/*
 * By M. Iqbal F. A.
 * Modified from:
 * file DFRobot_ESP_PH.h * @ https://github.com/GreenPonik/DFRobot_ESP_PH_BY_GREENPONIK
 *
 * Arduino library for Gravity: Analog pH Sensor / Meter Kit V2, SKU: SEN0161-V2
 * Compatible for ESP32
 */

#ifndef _ESP_PH_H_
#define _ESP_PH_H_

#include "ESP_Sensor.h"

#define PHVALUEADDR 0 //the start address of the pH calibration parameters stored in the EEPROM

#define NEUTRAL_VALUE 6.86
#define ACID_VALUE 4.01
#define PH_SENSOR 32 //pH sensor pin

class ESP_PH: public ESP_Sensor
{
    public:
        ESP_PH();
        ~ESP_PH();                        
        
    private:
        float _acidVoltage;
        float _neutralVoltage;
            
        float calculateValueFromVolt();
        float compensateVoltWithTemperature();
};

#endif
