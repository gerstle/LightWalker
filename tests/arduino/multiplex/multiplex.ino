#include <Wire.h>
#include <ADXL345.h>
#include <SPI.h>
#include <TCL.h>

#define TCA9548AADDR 0x74 //1110100

// For now, one object works...
ADXL345 adxl;
unsigned long lastStatus = millis();
double xyz[3];
 
void setup()
{
    Serial.begin(9600);

    Wire.begin();

    Serial.print("initing adxl0...");
    selectI2CChannels(0x1);
    adxl.powerOn();
    Serial.println("done");

    Serial.print("initing adxl1...");
    selectI2CChannels(0x2);
    adxl.powerOn();
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

    selectI2CChannels(0x1);
    adxl.getAccelemeter(xyz);
    if (printStatus)
        printXYZ("ONE");
 
    selectI2CChannels(0x2);
    adxl.getAccelemeter(xyz);
    if (printStatus)
        printXYZ("TWO");
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
