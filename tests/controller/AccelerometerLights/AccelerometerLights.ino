#include <Wire.h>
#include <ADXL345.h>
#include <SPI.h>
#include <TCL.h>
#include "colors.h"

ADXL345 adxl; //variable adxl is an instance of the ADXL345 library

unsigned long currentTime = millis();
int stepDuration = 250; // <cgerstle> a step lasts at least this long... ie, two steps can't occur within this time period
int samplePrecisionThreshold = 15;

const int valueCount = 25;
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

const int pixelCount = 250;
RGB pixels[pixelCount];

int aboveAverageCounter = 0; // <cgerstle> counts the number of cycles the zvalue is above average

int redIndex = 0;
int greenIndex = 1;
int blueIndex = 2;

unsigned long lastXSwitch = millis();
unsigned long lastYSwitch = millis();

void setPixel(RGB color)
{
    TCL.sendColor(color.r, color.g, color.b);
}

void setup() 
{ 
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

	double xyz[3];
    for (valueIndex = 0; valueIndex < valueCount; valueIndex++)
    {
        adxl.getAcceleration(xyz);
        xValues[valueIndex] = xyz[0];
        yValues[valueIndex] = xyz[1];
        zValues[valueIndex] = xyz[2];

        xTotal += xValues[valueIndex];
        yTotal += yValues[valueIndex];
        zTotal += zValues[valueIndex];
    }

    xAverage = xTotal / valueCount;
    yAverage = yTotal / valueCount;
    zAverage = zTotal / valueCount;
    zOld = zValues[valueIndex - 2];
    zNew = zValues[valueIndex - 1];

    TCL.begin();
    TCL.setupDeveloperShield();
    TCL.sendEmptyFrame();

    pixels[0].r = map(xValues[valueIndex], 0, 2.0, 0, 255);
    pixels[0].g = map(yValues[valueIndex], 0, 2.0, 0, 255);
    pixels[0].b = map(zValues[valueIndex], 0, 2.0, 0, 255);
    setPixel(pixels[0]);
    for (int i = 1; i < pixelCount; i++)
    {
        pixels[i].r = pixels[0].r;
        pixels[i].g = pixels[0].g;
        pixels[i].b = pixels[0].b;
        setPixel(pixels[i]);
    }
} 

bool canXSwitch = true;
bool canYSwitch = true;
void loop() 
{ 
    // <gerstle> first off, get values from sensor
	double xyz[3];
    adxl.getAcceleration(xyz);

    currentTime = millis();

    Serial.print(xyz[0]); Serial.print(","); Serial.print(xyz[1]); Serial.print(","); Serial.print(xyz[2]);
//         Serial.print(","); Serial.print(zAverage);// Serial.print(","); Serial.print(zMax);
        //Serial.print(","); Serial.print(aboveAverageCounter);
        //Serial.print(","); Serial.print(zMin);
        //Serial.println();
/*
    if (valueIndex >= valueCount)
        valueIndex = 0;

    if (z > zAverage)
        aboveAverageCounter++;
    else
        aboveAverageCounter = 0;
   
    zTotal = zTotal - zValues[valueIndex] + z;
    zValues[valueIndex] = z;
    zAverage = zTotal / valueCount;
*/

    TCL.sendEmptyFrame();

    if (redIndex == 2)
    {
        if (xyz[redIndex] > 0)
            pixels[0].r = map(abs(xyz[redIndex]) * 100, 0, 192, 0, 255);
        else
            pixels[0].r = 0x0;
    }
    else
        pixels[0].r = map(abs(xyz[redIndex]) * 100, 0, 192, 0, 255);

    if (greenIndex == 2)
    {
        if (xyz[greenIndex] > 0)
            pixels[0].g = map(abs(xyz[greenIndex]) * 100, 0, 192, 0, 255);
        else
            pixels[0].g = 0x0;
    }
    else
        pixels[0].g = map(abs(xyz[greenIndex]) * 100, 0, 192, 0, 255);

    if (blueIndex == 2)
    {
        if (xyz[blueIndex] > 0)
            pixels[0].b = map(abs(xyz[blueIndex]) * 100, 0, 192, 0, 255);
        else
            pixels[0].b = 0x0;
    }
    else
        pixels[0].b = map(abs(xyz[blueIndex]) * 100, 0, 192, 0, 255);


    if ((abs(xyz[0]) > 0.9) && canXSwitch)
    {
        if (redIndex == 0)
        {
                redIndex = 2;
                blueIndex = 0;
                greenIndex = 1;
        }
        else if (greenIndex == 0)
        {
                redIndex = 0;
                blueIndex = 1;
                greenIndex = 2;
        }
        else if (blueIndex == 0)
        {
                redIndex = 1;
                blueIndex = 2;
                greenIndex = 0;
        }
        canXSwitch = false;
        lastXSwitch = millis();
    }

    if ((abs(xyz[1]) > 0.97) && canYSwitch)
    {
        if (redIndex == 1)
        {
                redIndex = 2;
                blueIndex = 0;
                greenIndex = 1;
        }
        else if (greenIndex == 1)
        {
                redIndex = 0;
                blueIndex = 1;
                greenIndex = 2;
        }
        else if (blueIndex == 1)
        {
                redIndex = 1;
                blueIndex = 2;
                greenIndex = 0;
        }
        canYSwitch = false;
        lastYSwitch = millis();
    }

    if ((abs(xyz[0]) < 0.3) && (currentTime > (lastXSwitch + 600)))
        canXSwitch = true;

    if ((abs(xyz[1]) < 0.3) && (currentTime > (lastYSwitch + 600)))
        canYSwitch = true;

    Serial.print(","); Serial.print(pixels[0].r); Serial.print(","); Serial.print(pixels[0].g); Serial.print(","); Serial.println(pixels[0].b);
    setPixel(pixels[0]);
    for (int i = 1; i < pixelCount; i++)
    {
        pixels[i].r = pixels[0].r;
        pixels[i].g = pixels[0].g;
        pixels[i].b = pixels[0].b;
        setPixel(pixels[i]);
    }

    // <gerstle> step detection algorithms
    // detectStepAverageSpike(x, y, z, currentTime);

    // <gerstle> tracking data
//     zOld = zNew;
//     zNew = z;
}

unsigned long lastSharpPeakTime = millis();
int peakThreshold = 7;
int zAverageOld = 0;
void detectStepAverageSpike(int x, int y, int z, unsigned long currentTime)
{
    if ((currentTime > (lastSharpPeakTime + stepDuration)) &&
        (zAverage >= (zAverageOld + peakThreshold)))
    {
//         Serial.println(",300--------------------------------------- STEP!");
//         digitalWrite(greenPin, HIGH);
        lastSharpPeakTime = currentTime;
    }
    else if (currentTime > (lastSharpPeakTime + stepDuration))
    {
//         digitalWrite(greenPin, LOW);
//         Serial.println(",0");
    }
//     else
//         Serial.println(",1");

    zAverageOld = zAverage;
}
