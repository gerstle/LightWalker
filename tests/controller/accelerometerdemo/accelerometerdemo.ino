#include <Wire.h>
#include "ADXL345.h"

ADXL345 adxl1;
ADXL345 adxl2;
ADXL345 adxl3;
ADXL345 adxl4;

#define TCA9548AADDR 0x74 // 1110100
#define ADXL_ONE 0x1
#define ADXL_TWO 0x2
#define ADXL_THREE 0x4
#define ADXL_FOUR 0x8

bool one_on = true;
bool two_on = true;
bool three_on = true;
bool four_on = true;
 
void setup()
{
    Serial.begin(9600);
    delay(3000);

    Wire.begin();
    Serial.println("begin");

    if (one_on)
    {
        Serial.print("initing adxl 1...");
        selectI2CChannels(ADXL_ONE);
        Serial.print(" x ");
        initADXL(adxl1);
        Serial.println("done");
    }

    if (two_on)
    {
        Serial.print("initing adxl 2...");
        selectI2CChannels(ADXL_TWO);
        Serial.print(" x ");
        initADXL(adxl2);
        Serial.println("done");
    }

    if (three_on)
    {
        Serial.print("initing adxl 3...");
        selectI2CChannels(ADXL_THREE);
        Serial.print(" x ");
        initADXL(adxl3);
        Serial.println("done");
    }

    if (four_on)
    {
        Serial.print("initing adxl 4...");
        selectI2CChannels(ADXL_FOUR);
        Serial.print(" x ");
        initADXL(adxl4);
        Serial.println("done");
    }
}
 
void loop()
{
    double xyz[3];

    if (one_on)
    {
        selectI2CChannels(ADXL_ONE);
        adxl1.getAcceleration(xyz);
        Serial.print(xyz[0]); Serial.print(","); Serial.print(xyz[1]); Serial.print(","); Serial.println(xyz[2]);
    }

    if (two_on)
    {
        selectI2CChannels(ADXL_TWO);
        adxl2.getAcceleration(xyz);
        Serial.print(xyz[0]); Serial.print(","); Serial.print(xyz[1]); Serial.print(","); Serial.println(xyz[2]);
    }

    if (three_on)
    {
        selectI2CChannels(ADXL_THREE);
        adxl3.getAcceleration(xyz);
        Serial.print(xyz[0]); Serial.print(","); Serial.print(xyz[1]); Serial.print(","); Serial.println(xyz[2]);
    }

    if (four_on)
    {
        selectI2CChannels(ADXL_FOUR);
        adxl4.getAcceleration(xyz);
        Serial.print(xyz[0]); Serial.print(","); Serial.print(xyz[1]); Serial.print(","); Serial.println(xyz[2]);
    }

    Serial.println();
    delay(500);
}

void initADXL(ADXL345 adxl)
{
    Serial.print(":");
    adxl.powerOn();
    Serial.print(":");

    //set activity/ inactivity thresholds (0-255)
    adxl.setActivityThreshold(75); //62.5mg per increment
    adxl.setInactivityThreshold(75); //62.5mg per increment
    adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
    Serial.print(":");

    //look of activity movement on this axes - 1 == on; 0 == off 
    adxl.setActivityX(1);
    adxl.setActivityY(1);
    adxl.setActivityZ(1);
    Serial.print(":");

    //look of inactivity movement on this axes - 1 == on; 0 == off
    adxl.setInactivityX(1);
    adxl.setInactivityY(1);
    adxl.setInactivityZ(1);
    Serial.print(":");

    //look of tap movement on this axes - 1 == on; 0 == off
    adxl.setTapDetectionOnX(0);
    adxl.setTapDetectionOnY(0);
    adxl.setTapDetectionOnZ(0);
    Serial.print(":");

    //set values for what is considered freefall (0-255)
    adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
    adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
    Serial.print(":");
}

void selectI2CChannels(int channels) 
{
    //Serial.print(",");
    Wire.beginTransmission(TCA9548AADDR);
    //Serial.print(",");
    //Serial.print(Wire.write(channels));
    Wire.write(channels);
    //Serial.print(",");
    //Serial.println(Wire.endTransmission(I2C_NOSTOP, 500));
    //Serial.println(Wire.endTransmission());
    Wire.endTransmission();
    //Serial.println(Wire.done());
    //Serial.println(Wire.status());
    //Serial.print(",");
}

