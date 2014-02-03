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

int xPin = 7;
int yPin = 8;
int zPin = 12;

unsigned long currentTime = millis();
unsigned long lastSharpPeakTime = millis();

const int valueCount = 60;
int x, y, z;  
int xValues[valueCount];
int yValues[valueCount];
int zValues[valueCount];

int valueIndex = 0;

int xTotal = 0;
int xAverage = 0;
int xAverageOld = 0;

int yTotal = 0;
int yAverage = 0;
int yAverageOld = 0;

int zTotal = 0;
int zAverage = 0;
int zAverageOld = 0;

// int aboveAverageCounter = 0; // <cgerstle> counts the number of cycles the zvalue is above average
int xStepDuration = 750; // <cgerstle> a step lasts at least this long... ie, two steps can't occur within this time period
int yStepDuration = 750;
int zStepDuration = 250;
int xAvgDiffThreshold = 3;
int yAvgDiffThreshold = 3;
int zAvgDiffThreshold = 3;

int zSignificantlyLowerThanAverageThreshold = 35; 
bool readyForStep = true;

void setup() 
{ 
    pinMode(xPin, OUTPUT);
    pinMode(yPin, OUTPUT);
    pinMode(zPin, OUTPUT);

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

    for (valueIndex = 0; valueIndex < valueCount; valueIndex++)
    {
        adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

        x = abs(x);
        y = abs(y);
        z = z;

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

} 

bool stepDetected = false;
void loop() 
{ 
    currentTime = millis();

    adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

    x = abs(x);
    y = abs(y);
    z = z;

    if (valueIndex >= valueCount)
        valueIndex = 0;

//     if (z > zAverage)
//         aboveAverageCounter++;
//     else
//         aboveAverageCounter = 0;
    xTotal = xTotal - xValues[valueIndex] + x;
    xValues[valueIndex] = x;
    xAverage = xTotal / valueCount;

    yTotal = yTotal - yValues[valueIndex] + y;
    yValues[valueIndex] = y;
    yAverage = yTotal / valueCount;
   
    zTotal = zTotal - zValues[valueIndex] + z;
    zValues[valueIndex] = z;
    zAverage = zTotal / valueCount;

    // Serial.print("\t"); Serial.print(zAverage);

    //detectStepDownSlope(x, y, z, currentTime);

        // Serial.println();

    detectStepAverageSpike(currentTime);

    xAverageOld = xAverage;
    yAverageOld = yAverage;
    zAverageOld = zAverage;


    // <cgerstle> not having this may very well be why my shit works...
    valueIndex++;
}

// unsigned long lastDownSlopeStepTime = millis();
// int samplePrecisionThreshold = 15;
// void detectStepDownSlope(int x, int y, int z, unsigned long currentTime)
// {
//     // <gerstle> z takes precedence
//     if ((currentTime > (lastDownSlopeStepTime + stepDuration)) &&
//         (zOld >= zAverage) &&
//         ((z < zAverage) && (z <= (zOld - samplePrecisionThreshold))) &&
//         (aboveAverageCounter < 3))
//     {
//         digitalWrite(zPin, HIGH);
//         lastDownSlopeStepTime = currentTime;
//     }
//     else if (currentTime > (lastDownSlopeStepTime + stepDuration))
//         digitalWrite(zPin, LOW);
// }

bool detectStepAverageSpike(unsigned long currentTime)
{
    stepDetected = false;

    if (z < (zAverage - zSignificantlyLowerThanAverageThreshold))
        readyForStep = true;

    Serial.print(x); Serial.print("\t"); Serial.print(y); Serial.print("\t"); Serial.print(z);
        Serial.print("\t"); Serial.print(xAverage); Serial.print("\t"); Serial.print(xAverageOld);
        Serial.print("\t"); Serial.print(yAverage); Serial.print("\t"); Serial.print(yAverageOld);
        Serial.print("\t"); Serial.print(zAverage); Serial.print("\t"); Serial.print(zAverageOld);
        Serial.print("\t"); Serial.print(readyForStep);

    if (readyForStep && (currentTime > (lastSharpPeakTime + zStepDuration)))
        if ((z > zAverage) && (zAverage >= (zAverageOld + zAvgDiffThreshold)))
        {
            digitalWrite(xPin, LOW);
            digitalWrite(yPin, LOW);
            digitalWrite(zPin, HIGH);
            stepDetected = true;
//             Serial.println("\t0\t0\t300");
//             zAverage = zAverageOld;
//             zTotal = zTotal - zValues[valueIndex] + zAverageOld;
//             zValues[valueIndex] = zAverageOld;
            readyForStep = false;
        }
        else
            digitalWrite(zPin, LOW);


    if (readyForStep && (currentTime > (lastSharpPeakTime + xStepDuration)))
        if (!stepDetected && (xAverage >= (xAverageOld + xAvgDiffThreshold)))
        {
            digitalWrite(xPin, HIGH);
            stepDetected = true;
//             Serial.println("\t300\t0\t0");
        }
        else
            digitalWrite(xPin, LOW);

    if (readyForStep && (currentTime > (lastSharpPeakTime + yStepDuration)))
        if (!stepDetected && (yAverage >= (yAverageOld + yAvgDiffThreshold)))
        {
            digitalWrite(yPin, HIGH);
            stepDetected = true;
//             Serial.println("\t0\t300\t0");
        }
        else
            digitalWrite(yPin, LOW);

    if (stepDetected)
    {
        Serial.println("\t333");
        lastSharpPeakTime = millis();
    }
    else
        Serial.println("\t0");
//         Serial.println("\t0\t0\t0");
}
