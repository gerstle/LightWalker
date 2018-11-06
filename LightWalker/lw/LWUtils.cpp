/*****************************************************************************
 * LWUtils.cpp
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include <math.h>
#include "LWUtils.h"
#include "../config/LWConfigs.h"
#include <i2c_t3.h>

LWUtilsClass LWUtils;

// <gerstle> debounce an input switch
bool LWUtilsClass::debounce(int switchPin, bool lastState)
{
    bool currentState = digitalRead(switchPin);

    if (lastState != currentState)
    {
        delay(5);
        currentState = digitalRead(switchPin);
    }

    return currentState;
}

void LWUtilsClass::selectI2CChannels(int channels) 
{
    Wire1.beginTransmission(I2C_MULTIPLEXER);
    Wire1.write(channels);
    Wire1.endTransmission();
}

void LWUtilsClass::initADXL(ADXL345 *adxl)
{
    adxl->powerOn();
    adxl->setRangeSetting(2);

    //set activity/ inactivity thresholds (0-255)
    adxl->setActivityThreshold(75); //62.5mg per increment
    adxl->setInactivityThreshold(75); //62.5mg per increment
    adxl->setTimeInactivity(10); // how many seconds of no activity is inactive?

    //look of activity movement on this axes - 1 == on; 0 == off 
    adxl->setActivityX(1);
    adxl->setActivityY(1);
    adxl->setActivityZ(1);

    //set values for what is considered freefall (0-255)
    adxl->setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
    adxl->setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
}

void LWUtilsClass::printRGB(CRGB color, bool newLine)
{
    printRGB((byte)color.r, (byte)color.g, (byte)color.b, newLine);
}

void LWUtilsClass::printRGB(byte r, byte g, byte b, bool newLine)
{
    Serial.print(r); Serial.print(", "); Serial.print(g); Serial.print(", "); Serial.print(b);

    if (newLine)
        Serial.println();
}

void LWUtilsClass::setTransitionColor(CRGB* led, double numerator, double denomenator, CHSV fromColor, CHSV toColor)
{
    CRGB from = fromColor;
    CRGB to = toColor;

    LWUtilsClass::setTransitionColor(led, numerator, denomenator, from, to);
}

void LWUtilsClass::setTransitionColor(CRGB* led, double numerator, double denomenator, CRGB fromColor, CRGB toColor)
{
    if (numerator <= 0)
        *led = fromColor;
    else if (numerator >= denomenator)
        *led = toColor;
    else
    {
        CRGB toDelta = toColor;
        toDelta %= (byte)(255 * ((float)numerator/(float)denomenator));

        CRGB fromDelta = fromColor;
        fromDelta %= (byte)(255 * ((float)(denomenator - numerator)/(float)denomenator));

        *led = CRGB::Black;
        *led += fromDelta;
        *led += toDelta;
    }
}

/*
 Ken Perlins improved noise   -  http://mrl.nyu.edu/~perlin/noise/
 C-port:  http://www.fundza.com/c4serious/noise/perlin/perlin.html
 by Malcolm Kesson;   arduino port by Peter Chiochetti, Sep 2007 :
 -  make permutation constant byte, obsoletes init(), lookup % 256
*/

static const byte p[] = {   151,160,137,91,90, 15,131, 13,201,95,96,
53,194,233, 7,225,140,36,103,30,69,142, 8,99,37,240,21,10,23,190, 6,
148,247,120,234,75, 0,26,197,62,94,252,219,203,117, 35,11,32,57,177,
33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,
48,27,166, 77,146,158,231,83,111,229,122, 60,211,133,230,220,105,92,
41,55,46,245,40,244,102,143,54,65,25,63,161, 1,216,80,73,209,76,132,
187,208, 89, 18,169,200,196,135,130,116,188,159, 86,164,100,109,198,
173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,
212,207,206, 59,227, 47,16,58,17,182,189, 28,42,223,183,170,213,119,
248,152,2,44,154,163,70,221,153,101,155,167,43,172, 9,129,22,39,253,
19,98,108,110,79,113,224,232,178,185,112,104,218,246, 97,228,251,34,
242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,
150,254,138,236,205, 93,222,114, 67,29,24, 72,243,141,128,195,78,66,
215,61,156,180
};

double fade(double t){ return t * t * t * (t * (t * 6 - 15) + 10); }
double lerp(double t, double a, double b){ return a + t * (b - a); }
double grad(int hash, double x, double y, double z)
{
    int     h = hash & 15;          /* CONVERT LO 4 BITS OF HASH CODE */
    double  u = h < 8 ? x : y,      /* INTO 12 GRADIENT DIRECTIONS.   */
            v = h < 4 ? y : h==12||h==14 ? x : z;
    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

#define P(x) p[(x) & 255]

double LWUtilsClass::perlinNoise(double x, double y, double z)
{
    int   X = (int)floor(x) & 255,             /* FIND UNIT CUBE THAT */
          Y = (int)floor(y) & 255,             /* CONTAINS POINT.     */
          Z = (int)floor(z) & 255;
    x -= floor(x);                             /* FIND RELATIVE X,Y,Z */
    y -= floor(y);                             /* OF POINT IN CUBE.   */
    z -= floor(z);
    double  u = fade(x),                       /* COMPUTE FADE CURVES */
            v = fade(y),                       /* FOR EACH OF X,Y,Z.  */
            w = fade(z);
    int  A = P(X)+Y, 
         AA = P(A)+Z, 
         AB = P(A+1)+Z,                        /* HASH COORDINATES OF */
         B = P(X+1)+Y, 
         BA = P(B)+Z, 
         BB = P(B+1)+Z;                        /* THE 8 CUBE CORNERS, */

    return lerp(w,lerp(v,lerp(u, grad(P(AA  ), x, y, z),   /* AND ADD */
                              grad(P(BA  ), x-1, y, z)),   /* BLENDED */
                  lerp(u, grad(P(AB  ), x, y-1, z),        /* RESULTS */
                       grad(P(BB  ), x-1, y-1, z))),       /* FROM  8 */
                lerp(v, lerp(u, grad(P(AA+1), x, y, z-1),  /* CORNERS */
                     grad(P(BA+1), x-1, y, z-1)),          /* OF CUBE */
                  lerp(u, grad(P(AB+1), x, y-1, z-1),
                       grad(P(BB+1), x-1, y-1, z-1))));
}
