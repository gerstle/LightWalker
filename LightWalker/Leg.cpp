/*****************************************************************************
 * Leg.cpp
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include "Leg.h"

void Leg::init(LWConfigs *c, char *n, int i2c_channel, WalkingModeEnum mode, ADXL345 *adxl, byte count, byte half, CRGB *p)
{
    _config = c;
    name = n;
    _channel = i2c_channel;
    _pixels = p;
    _adxl = adxl;
    _pixelCount = count;
    _half = half;

    Serial.print(name); Serial.println(" initialized with:");
    Serial.print("    pixel count: "); Serial.println(_pixelCount);
    Serial.print("    half: "); Serial.println(_half);

    LWUtils.selectI2CChannels(_channel);
    LWUtils.initADXL(_adxl);

    // <gerstle> init ADXL EMA's
    short x, y, z;
    int valueIndex = 0;
    int xValueTotal, yValueTotal, zValueTotal  = 0;
    for (valueIndex = 0; valueIndex < ADXL_VALUE_COUNT; valueIndex++)
    {
        _adxl->readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

        x = x;
        y = abs(y);
        z = abs(z);

        xValueTotal += x;
        yValueTotal += y;
        zValueTotal += z;

        xNMinus2 = xNMinus1;
        xNMinus1 = x;

        yNMinus2 = yNMinus1;
        yNMinus1 = y;

        zNMinus2 = zNMinus1;
        zNMinus1 = z;
    }

    xEMA = xValueTotal / ADXL_VALUE_COUNT;
    yEMA = yValueTotal / ADXL_VALUE_COUNT;
    zEMA = zValueTotal / ADXL_VALUE_COUNT;
}

void Leg::setWalkingMode(WalkingModeEnum mode)
{
    _walkingMode = mode;
    off();

    switch (_walkingMode)
    {
        case masterOff:
            off();
            break;

        case pulse:
            _leg_mode = &pulseLegMode;
            break;

        case sparkle:
            _leg_mode = &sparkleLegMode;
            break;

        case equalizer:
            _leg_mode = &equalizerLegMode;
            break;

        case gravity:
            _leg_mode = &gravityLegMode;
            break;

        case bubble:
            _leg_mode = &bubbleLegMode;
            break;

        case rainbow:
            _leg_mode = &rainbowLegMode;
            break;
    }

    if (_leg_mode != NULL)
        _leg_mode->setup(_config, name, _channel, _adxl, _pixelCount, _half, _pixels);
}

void Leg::frame()
{
    // <gerstle> the switch handles and frame specific
    // needs for each of the modes
    switch (_walkingMode)
    {
        case pulse:
            break;
        case sparkle:
        case bubble:
            _leg_mode->stepDetected = detectStep();
            break;

        default:
            break;
    }

    _leg_mode->frame();
}

bool Leg::detectStep()
{
    short x, y, z;
    bool step_detected = false;

    LWUtils.selectI2CChannels(_channel);
    _adxl->readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

    x = x;
    y = abs(y);
    z = abs(z);

    xEMA = (float)(x- xEMA) * ((float)2 / (float)(ADXL_VALUE_COUNT + 1)) + xEMA;
    yEMA = (float)(y- yEMA) * ((float)2 / (float)(ADXL_VALUE_COUNT + 1)) + yEMA;
    zEMA = (float)(z- zEMA) * ((float)2 / (float)(ADXL_VALUE_COUNT + 1)) + zEMA;

    xNMinus2 = xNMinus1;
    xNMinus1 = x;

    yNMinus2 = yNMinus1;
    yNMinus1 = y;

    zNMinus2 = zNMinus1;
    zNMinus1 = z;

/*
    Serial.print(x); Serial.print("\t"); Serial.print(y); Serial.print("\t"); Serial.print(z);
    Serial.print("\t"); Serial.print(xEMA);
    Serial.print("\t"); Serial.print(yEMA);
    Serial.print("\t"); Serial.print(zEMA);
    Serial.print("\t"); Serial.println(readyForStep);
*/

    if (x < (xEMA - xSignificantlyLowerThanAverageThreshold))
        readyForStep = true;

    if (readyForStep && (lastSharpPeakTime >= xStepDuration))
    {
        lastSharpPeakTime = 0;
        if (x > (xEMA + xAvgDiffThreshold))
        {
            step_detected = true;
            readyForStep = false;
            //Serial.print("X\t\t");
        }
    }

    if (readyForStep && (lastSharpPeakTime >= yStepDuration))
    {
        lastSharpPeakTime = 0;
        if (!step_detected && (y >= (yEMA + yAvgDiffThreshold)))
        {
            step_detected = true;
            //Serial.print("y\t\t");
        }
    }


    if (readyForStep && (lastSharpPeakTime >= zStepDuration))
    {
        lastSharpPeakTime = 0;
        if (!step_detected && (z >= (zEMA + zAvgDiffThreshold)))
        {
            step_detected = true;
            //Serial.print("Z\t\t");
        }
    }

    if (step_detected)
        return true;
    else
        return false;
}

void Leg::off()
{
    for (int i = 0; i < _pixelCount; i++)
        _pixels[i] = CRGB::Black;
}
