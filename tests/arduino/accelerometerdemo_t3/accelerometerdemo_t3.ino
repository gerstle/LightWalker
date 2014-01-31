#include <i2c_t3.h>
#include <ADXL345_t3.h>

ADXL345 adxl;

#define TCA9548AADDR 0x74 // 1110100
#define ADXL_ONE 0x1
 
void setup()
{
    Serial.begin(9600);

//     Wire.begin();
// 
//     Serial.print("initing adxl0...");
//     selectI2CChannels(ADXL_ONE);
//     initADXL(adxl);
//     Serial.println("done");
}
 
byte a = 254;
byte b = 253;
short y;
void loop()
{
//     double xyz[3];
//     adxl.getAcceleration(xyz);
    //Serial.print(xyz[0]); Serial.print(","); Serial.print(xyz[1]); Serial.print(","); Serial.println(xyz[2]);

    Serial.print(a, BIN); Serial.print("\t");
    Serial.print(b, BIN); Serial.print("\t");
	y = (((short)a) << 8) | (short)b;
    Serial.print(y, BIN); Serial.print("\t");
    Serial.println(y);

    delay(500);
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

void selectI2CChannels(int channels) 
{
    Wire.beginTransmission(TCA9548AADDR);
    Wire.write(channels);
    Wire.endTransmission();  
}
