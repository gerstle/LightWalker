// Pretty decent first attempt at a step detect algorithm.
//
// Detecting most everything on just the z access, but I am
// getting a fair amount of false positives. I'm doing a lot of work to
// capture a good moving average of z values, but not really 
// doing that much with them. I should change to just using a max + min / 2
// scheme or get to relying on the average more. In graphing the data I just
// don't really see much of a justification for the reliance on an accurate
// average. Maybe If I did more of a threshold for steps based on the average, 
// I could get better detection on both hard and light steps?
// 
// Basic setup is a single LED hooked to pin 13 and an ADXL345 hooked to i2c


#include <Wire.h>
#include <ADXL345.h>

ADXL345 adxl; //variable adxl is an instance of the ADXL345 library

int ledPin = 13;

unsigned long lastStepTime = millis();
unsigned long currentTime = millis();
int stepDuration = 250; // <cgerstle> a step lasts at least this long... ie, two steps can't occur within this time period
int samplePrecisionThreshold = 15;

const int valueCount = 20;
int xValues[valueCount];
int yValues[valueCount];
int zValues[valueCount];

int valueIndex = 0;

int xTotal = 0;
int xAverage = 0;
// int xMax = 0;
// int xMin = 0;

int yTotal = 0;
int yAverage = 0;
// int yMax = 0;
// int yMin = 0;

int zTotal = 0;
int zAverage = 0;
// int zMax = 0;
// int zMin = 0;
int zOld = 0;
int zNew = 0;

int aboveAverageCounter = 0; // <cgerstle> counts the number of cycles the zvalue is above average

void setup() 
{ 
    pinMode(ledPin, OUTPUT);

    randomSeed(analogRead(0)); //initialize random seed

    Serial.begin(9600);
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

    int x,y,z;  
    for (valueIndex = 0; valueIndex < valueCount; valueIndex++)
    {
        adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
        xValues[valueIndex] = x;
        yValues[valueIndex] = y;
        zValues[valueIndex] = z;

        xTotal += xValues[valueIndex];
        yTotal += yValues[valueIndex];
        zTotal += zValues[valueIndex];
    }

    xAverage = xTotal / valueCount;
    yAverage = yTotal / valueCount;
    zAverage = zTotal / valueCount;
    zOld = zValues[valueIndex - 2];
    zNew = zValues[valueIndex - 1];
} 

void loop() 
{ 
    // <gerstle> first off, get values from sensor
    int x,y,z;  
    adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
    currentTime = millis();

    /*Serial.print(x); Serial.print(","); Serial.print(y); Serial.print(","); */Serial.print(z);
        Serial.print(","); Serial.print(zAverage);// Serial.print(","); Serial.print(zMax);
        Serial.print(","); Serial.print(aboveAverageCounter);
        //Serial.print(","); Serial.print(zMin);
        //Serial.println();


    if ((currentTime > (lastStepTime + stepDuration)) &&
        //(zOld < (zAverage + samplePrecisionThreshold)) &&
        (zNew >= zAverage) && //(zNew < (zOld - samplePrecisionThreshold))) &&
        ((z < zAverage) && (z <= (zNew - samplePrecisionThreshold))) &&
        (aboveAverageCounter < 3))
    {
        digitalWrite(ledPin, HIGH);
        Serial.println(",300---------------------------------- STEP!");
        lastStepTime = currentTime;
    }
    else if (currentTime > (lastStepTime + stepDuration))
    {
        digitalWrite(ledPin, LOW);
        Serial.println(",0");
    }
    else
        Serial.println(",1");

    if (z > zAverage)
        aboveAverageCounter++;
    else
        aboveAverageCounter = 0;

    zOld = zNew;
    zNew = z;

    if (valueIndex >= valueCount)
        valueIndex = 0;
   
    zTotal = zTotal - zValues[valueIndex] + zNew;
    zValues[valueIndex] = zNew;
    zAverage = zTotal / valueCount;

}
