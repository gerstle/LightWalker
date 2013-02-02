// Pretty decent first attempt at a step detect algorithm.
//
// 31jan2013 - Detecting most everything on just the z access, but I am
// getting a fair amount of false positives. I'm doing a lot of work to
// capture a good moving average of z values, but not really 
// doing that much with them. I should change to just using a max + min / 2
// scheme or get to relying on the average more. In graphing the data I just
// don't really see much of a justification for the reliance on an accurate
// average. Maybe If I did more of a threshold for steps based on the average, 
// I could get better detection on both hard and light steps?
//
// 2feb2013 - OK, had some time to think about this for a bit and I think I was
// overcomplicating. I was also detecting the step _after_ it physically happened.
// Not ideal. So, code that I wrote on 31jan2013 got moved into a detectStepDownSlope
// function and I wrote a new one that just looks at a significant increase in the 
// moving average. The thinking is that when the foot is hovering or at the top
// of it's movement, the average increases rather slowly. When the foot hits the
// ground, there's a significant spike in value and this single value causes
// the average to jump significantly. This gets rid of a lot of noise and also 
// detects the step closer to when the foot physically hits the ground. So,
// now I have the detectStepAverageSpike function.
// 
// Basic setup is a red LED hooked to pin 8 (with 330 ohm resistor) and a green
// LED hooked up to pin 12 (with 330 ohm resistor) and an ADXL345 hooked to i2c


#include <Wire.h>
#include <ADXL345.h>

ADXL345 adxl; //variable adxl is an instance of the ADXL345 library

int redPin = 8;
int greenPin = 12;

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

int aboveAverageCounter = 0; // <cgerstle> counts the number of cycles the zvalue is above average

void setup() 
{ 
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);

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

//     Serial.print(x); Serial.print(","); Serial.print(y); Serial.print(","); Serial.print(z);
//         Serial.print(","); Serial.print(zAverage);// Serial.print(","); Serial.print(zMax);
        //Serial.print(","); Serial.print(aboveAverageCounter);
        //Serial.print(","); Serial.print(zMin);
        //Serial.println();

    if (valueIndex >= valueCount)
        valueIndex = 0;

    if (z > zAverage)
        aboveAverageCounter++;
    else
        aboveAverageCounter = 0;
   
    zTotal = zTotal - zValues[valueIndex] + z;
    zValues[valueIndex] = z;
    zAverage = zTotal / valueCount;

//     Serial.print(","); Serial.print(zAverage);

    // <gerstle> step detection algorithms
    detectStepDownSlope(x, y, z, currentTime);
    detectStepAverageSpike(x, y, z, currentTime);

    // <gerstle> tracking data
    zOld = zNew;
    zNew = z;

}

unsigned long lastDownSlopeStepTime = millis();
void detectStepDownSlope(int x, int y, int z, unsigned long currentTime)
{
    if ((currentTime > (lastDownSlopeStepTime + stepDuration)) &&
        //(zOld < (zAverage + samplePrecisionThreshold)) &&
        (zNew >= zAverage) && //(zNew < (zOld - samplePrecisionThreshold))) &&
        ((z < zAverage) && (z <= (zNew - samplePrecisionThreshold))) &&
        (aboveAverageCounter < 3))
    {
        digitalWrite(redPin, HIGH);
//         Serial.println(",300---------------------------------- STEP!");
        lastDownSlopeStepTime = currentTime;
    }
    else if (currentTime > (lastDownSlopeStepTime + stepDuration))
    {
        digitalWrite(redPin, LOW);
//         Serial.println(",0");
    }
//     else
//         Serial.println(",1");
}

unsigned long lastSharpPeakTime = millis();
int peakThreshold = 10;
int zAverageOld = 0;
void detectStepAverageSpike(int x, int y, int z, unsigned long currentTime)
{
    if ((currentTime > (lastSharpPeakTime + stepDuration)) &&
        (zAverage >= (zAverageOld + peakThreshold)))
    {
//         Serial.println(",300--------------------------------------- STEP!");
        digitalWrite(greenPin, HIGH);
        lastSharpPeakTime = currentTime;
    }
    else if (currentTime > (lastSharpPeakTime + stepDuration))
    {
        digitalWrite(greenPin, LOW);
//         Serial.println(",0");
    }
//     else
//         Serial.println(",1");

    zAverageOld = zAverage;
}
