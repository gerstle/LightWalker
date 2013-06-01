#include <Wire.h>
#include <ADXL345.h>
#include <SPI.h>
#include <TCL.h>

#define TCA9548AADDR 0x74 // 1110100

#define ADXL_ONE 0x1
#define ADXL_TWO 0x2
#define ADXL_THREE 0x4
#define ADXL_FOUR 0x8

// For now, one object works...
ADXL345 adxl;
unsigned long lastStatus = millis();
double xyz[3];
 
void setup()
{
    Serial.begin(9600);

    Wire.begin();

    Serial.print("initing adxl0...");
    selectI2CChannels(ADXL_ONE);
    initADXL(adxl);
    Serial.println("done");

    Serial.print("initing adxl1...");
    selectI2CChannels(ADXL_TWO);
    initADXL(adxl);
    Serial.println("done");

    Serial.print("initing adxl3...");
    selectI2CChannels(ADXL_THREE);
    initADXL(adxl);
    Serial.println("done");

    Serial.print("initing adxl4...");
    selectI2CChannels(ADXL_FOUR);
    initADXL(adxl);
    Serial.println("done");
}
 
void loop()
{
    unsigned long currentTime = millis();
    bool printStatus = false;

    if (currentTime > (lastStatus + 500))
    {
        printStatus = true;
        lastStatus = currentTime;
    }

    selectI2CChannels(ADXL_ONE);
    adxl.getAccelemeter(xyz);
    if (printStatus)
        printXYZ("  ONE");
 
    selectI2CChannels(ADXL_TWO);
    adxl.getAccelemeter(xyz);
    if (printStatus)
        printXYZ("  TWO");
 
    selectI2CChannels(ADXL_THREE);
    adxl.getAccelemeter(xyz);
    if (printStatus)
        printXYZ("THREE");
 
    selectI2CChannels(ADXL_FOUR);
    adxl.getAccelemeter(xyz);
    if (printStatus)
        printXYZ(" FOUR");
}
 
void selectI2CChannels(int channels) 
{
    Wire.beginTransmission(TCA9548AADDR);
    Wire.write(channels);
    Wire.endTransmission();  
}

void printXYZ(char label[])
{
    Serial.print(label); Serial.print(": ");
    Serial.print(xyz[0]); Serial.print(",");
    Serial.print(xyz[1]); Serial.print(",");
    Serial.println(xyz[2]);
}

void initADXL(ADXL345 adxl)
{
    adxl.powerOn();

    //set activity/ inactivity thresholds (0-255)
    adxl.setActivityThreshold(75); //62.5mg per increment
    adxl.setInactivityThreshold(75); //62.5mg per increment
    adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?

    //look of activity movement on this axes - 1 == on; 0 == off 
    adxl.setActivityX(1);
    adxl.setActivityY(1);
    adxl.setActivityZ(1);

    //look of inactivity movement on this axes - 1 == on; 0 == off
    adxl.setInactivityX(1);
    adxl.setInactivityY(1);
    adxl.setInactivityZ(1);

    //look of tap movement on this axes - 1 == on; 0 == off
    adxl.setTapDetectionOnX(0);
    adxl.setTapDetectionOnY(0);
    adxl.setTapDetectionOnZ(0);

    //set values for what is considered freefall (0-255)
    adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
    adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
}
