#ifndef _ESP_SENSOR_H_
#define _ESP_SENSOR_H_

// GENERAL
#include <Arduino.h>
#include <math.h>
#include <EEPROM.h>
//

// PI COMMAND -> SENSOR DATA
#include <DallasTemperature.h>
#include <OneWire.h>
//

// ONSITE INPUT
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
//

// ONSITE OUTPUT
#include "debounceButton.h"
//

// ONSITE (CALIBRATION)
#define CALCULATE_PERIOD 5000U
//

// PI COMMAND -> SENSOR DATA
#define ONE_WIRE_BUS 4 // temperature sensor
//

class ESP_Sensor
{
public:
    ESP_Sensor();
    ~ESP_Sensor();

    void calibration(byte *state);
    void updateVoltAndValue();
    void begin();
    void saveNewConfig();
    void saveNewCalib();
    void displayTwoLines(String firstLine, String secondLine);

    float _value;
    float _temperature;
    String _sensorName;
    String _sensorUnit;
    bool _resetCalibratedValueToDefault = 0;
    int _calibParamCount; // the amount of value in eeprom array for each sensor

    virtual bool isTbdOutOfRange();

    bool _enableSensor;

    struct eepromCalibParam
    {
        float solutionValue;
        float *calibVolt;
    } _calibParamArray[3];

protected:
    float _voltage;
    int _eepromStartAddress;
    int _eepromAddress;
    int _sensorPin;

    void calibDisplay(byte calibParamIdx);
    void captureCalibVolt(bool *calibrationFinish, byte calibParamIdx);
    void saveCalibVoltAndExit(bool *calibrationFinish);

    virtual float compensateVoltWithTemperature();
    virtual void readAndAverageVolt(); // to facilitate EC difference
    virtual float calculateValueFromVolt() = 0;
};

#endif
