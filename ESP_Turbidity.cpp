/*
 * By M. Iqbal F. A.
 * Modified from:
 * file DFRobot_ESP_EC.cpp * @ https://github.com/GreenPonik/DFRobot_ESP_EC_BY_GREENPONIK
 * file DFRobot_ESP_PH.cpp * @ https://github.com/GreenPonik/DFRobot_ESP_PH_BY_GREENPONIK
 *
 * Arduino library for Gravity: Analog Turbidity Sensor
 * Compatible for ESP32
 */

#include "ESP_Turbidity.h"

#define TBDVALUEADDR 0x14
#define TRANSPARENT_HIGH_VOLTAGE 3300.0
#define TRANSPARENT_LOW_VOLTAGE 2680.0
#define TRANSLUCENT_HIGH_VOLTAGE 2673.0
#define TRANSLUCENT_LOW_VOLTAGE 2455.0
#define OPAQUE_HIGH_VOLTAGE 2449.0
#define OPAQUE_LOW_VOLTAGE 2138.0
#define TRANSPARENT_VALUE 0.0
#define TRANSLUCENT_VALUE 1000.0
#define OPAQUE_VALUE 2000.0
#define TBD_SENSOR 32 //turbidity sensor pin

extern OneWire oneWire;// Setup a oneWire instance to communicate with any OneWire devices
extern DallasTemperature tempSensor;// Pass our oneWire reference to Dallas Temperature sensor 
extern LiquidCrystal_I2C lcd;
extern debounceButton cal_button;
extern debounceButton mode_button;

ESP_Turbidity::ESP_Turbidity()
{
    _eepromStartAddress = TBDVALUEADDR;
    
    //default values
    _transparentVoltage = 2772;   //solution 0.0 NTU at 25C
    _translucentVoltage = 2574; //solution 1000.0 NTU at 25C
    _opaqueVoltage = 2317; //solution 2000.0 NTU at 25C
    _calibSolutionArr[0] = {"Opaque (2000 NTU) Voltage", OPAQUE_VALUE, &_opaqueVoltage, OPAQUE_LOW_VOLTAGE, OPAQUE_HIGH_VOLTAGE};
    _calibSolutionArr[1] = {"Translucent (1000 NTU) Voltage", TRANSLUCENT_VALUE, &_translucentVoltage, TRANSLUCENT_LOW_VOLTAGE, TRANSLUCENT_HIGH_VOLTAGE};
    _calibSolutionArr[2] = {"Transparent (0 NTU) Voltage", TRANSPARENT_VALUE, &_transparentVoltage, TRANSPARENT_LOW_VOLTAGE, TRANSPARENT_HIGH_VOLTAGE};
    
    _vPeak = 1690;

    _paramName = "Tbd";
    _eepromN = 3;
    _unit = "NTU";
    _sensorPin = TBD_SENSOR;

    _isTempCompAcq = true;
}

ESP_Turbidity::~ESP_Turbidity()
{
}

bool ESP_Turbidity::isTbdOutOfRange()
{
    if (_voltage < _vPeak)
    {
        if (isnan(_value))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

float ESP_Turbidity::compensateRaw()
{
    float value = 0;
    float x1 = _opaqueVoltage; //opaque voltage
    float y1 = OPAQUE_VALUE; //opaque NTU
    float x2 = _translucentVoltage; //translucent voltage
    float y2 = TRANSLUCENT_VALUE; //translucent NTU
    float x3 = _transparentVoltage;
    float y3 = TRANSPARENT_VALUE; //transparent NTU
    
    float denom = (x1-x2) * (x1-x3) * (x2-x3);
    float a = (x3 * (y2-y1) + x2 * (y1-y3) + x1 * (y3-y2)) / denom;
    float b = (x3*x3 * (y1-y2) + x2*x2 * (y3-y1) + x1*x1 * (y2-y3)) / denom;
    float c = (x2 * x3 * (x2-x3) * y1+x3 * x1 * (x3-x1) * y2+x1 * x2 * (x1-x2) * y3) / denom;
    
    _vPeak = -b/(2*a);
    float ntu_peak = a*pow(_vPeak, 2.0) + b*_vPeak + c;
    //compensate voltage with temperature
    _voltage = (1455*_temperature-3795*_voltage+94875)/(2*_temperature+1405);
    if (isTbdOutOfRange())
    {
        value = ntu_peak;
        Serial.println("TBD Voltage Out of Range");
    }
    else
    {
        value = a*pow(_voltage, 2.0) + b*_voltage + c; //y = a*x^2 + b*x + c
    }
    return value;
}

void ESP_Turbidity::calibStartMessage()
{
    Serial.println();
    Serial.println(F(">>>Enter Turbidity Calibration Mode<<<"));
    Serial.println(F(">>>Please put the probe into the 0.0, 1000.0, or 2000.0 NTU standard solution.<<<"));
    Serial.println(F(">>>Calibrate all for best result.<<<"));
    Serial.println();
}