/*****************************************************************************
 * Leg.cpp
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include "Leg.h"

Leg::Leg()
{
	lastStepTimer = millis();
	xStepDuration = 150;
	yStepDuration = 350;
	zStepDuration = 350;
	xAvgDiffThreshold = 170;
	yAvgDiffThreshold = 150;
	zAvgDiffThreshold = 150;
	xSignificantlyLowerThanAverageThreshold = 45;
	readyForStep = true;
}

void Leg::init(config::LWConfigs *c, char const *n, int i2c_channel, WalkingModeEnum mode, ADXL345 *adxl, byte count, byte half, CRGB *p, int octoIndex)
{
    _config = c;
    name = n;
    _channel = i2c_channel;
    pixels = p;
    _adxl = adxl;
    pixelCount = count;
    _half = half;
    baseIndex = octoIndex;

//    LWUtils.selectI2CChannels(_channel);
//    LWUtils.initADXL(_adxl);

    Serial.print(name); Serial.println(" initialized with:");
    Serial.print("    pixel count: "); Serial.println(pixelCount);
    Serial.print("    half: "); Serial.println(_half);

//    // <gerstle> init ADXL EMA's
//    short x, y, z;
//    int valueIndex = 0;
//    double xValueTotal = 0;
//    double yValueTotal = 0;
//    double zValueTotal  = 0;
//    Serial.println("    initializing accelerometer");
//    for (valueIndex = 0; valueIndex < ADXL_VALUE_COUNT; valueIndex++)
//    {
//        _adxl->readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
//
//        // x = x;
//        y = abs(y);
//        z = abs(z);
//
//        xValueTotal += x;
//        yValueTotal += y;
//        zValueTotal += z;
//
//        xNMinus2 = xNMinus1;
//        xNMinus1 = x;
//
//        yNMinus2 = yNMinus1;
//        yNMinus1 = y;
//
//        zNMinus2 = zNMinus1;
//        zNMinus1 = z;
//    }
//
//    xEMA = xValueTotal / ADXL_VALUE_COUNT;
//    yEMA = yValueTotal / ADXL_VALUE_COUNT;
//    zEMA = zValueTotal / ADXL_VALUE_COUNT;
//    Serial.println("    done.");
}

void Leg::setWalkingMode(WalkingModeEnum mode)
{
    _walkingMode = mode;
    off();

//    switch (_walkingMode)
//    {
//        case masterOff:
//            _leg_mode = NULL;
//            off();
//            break;
//
//        case pulse:
//            _leg_mode = &pulseLegMode;
//            break;
//
//        case sparkle:
//            _leg_mode = &sparkleLegMode;
//            break;
//
//        case equalizer:
//            _leg_mode = &equalizerLegMode;
//            break;
//
//        case gravity:
//            _leg_mode = &gravityLegMode;
//            break;
//
//        case bubble:
//            _leg_mode = &bubbleLegMode;
//            break;
//
//        case rainbow:
//            _leg_mode = &rainbowLegMode;
//            break;
//
//        case zebra:
//            _leg_mode = &zebraLegMode;
//            break;
//
//        case chaos:
//            _leg_mode = &chaosLegMode;
//            break;
//
//        case flames:
//            _leg_mode = &flamesLegMode;
//        	break;
//    }

    if (_leg_mode != NULL)
        _leg_mode->setup(_config, name, _channel, _adxl, pixelCount, _half, pixels, baseIndex);
}

void Leg::frame()
{
    // <gerstle> the switch handles and frame specific
    // needs for each of the modes
    switch (_walkingMode)
    {
        case sparkle:
        case bubble:
        case rainbow:
        case zebra:
        case chaos:
        case flames:
            _leg_mode->stepDetected = detectStep();
            break;

        default:
            break;
    }

    if (_leg_mode != NULL)
        _leg_mode->frame();
}

bool Leg::detectStep()
{
    short x, y, z;
    bool step_detected = false;
    unsigned long currentTime = millis();

    //LWUtils.selectI2CChannels(_channel);
    _adxl->readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

    // x = x;
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

//     if (_channel == ADXL_TWO)
//     {
//         Serial.print(x); Serial.print("\t"); Serial.print(y); Serial.print("\t"); Serial.print(z);
//         Serial.print("\t"); Serial.print(xEMA);
//         Serial.print("\t"); Serial.print(yEMA);
//         Serial.print("\t"); Serial.print(zEMA);
//         Serial.print("\t"); Serial.print(readyForStep);
//         Serial.print("\t"); Serial.print(lastStepTimer);
//     }

    if (x < (xEMA - xSignificantlyLowerThanAverageThreshold))
        readyForStep = true;

    //if (readyForStep && (lastStepTimer >= xStepDuration))
    if (readyForStep && (currentTime >= (lastStepTimer + xStepDuration)))
    {
        if (x > (xEMA + xAvgDiffThreshold))
        {
            lastStepTimer = millis();
            step_detected = true;
            readyForStep = false;
        }
    }

    if (readyForStep && (currentTime >= (lastStepTimer + yStepDuration)))
    {
        if (!step_detected && (y >= (yEMA + yAvgDiffThreshold)))
        {
            lastStepTimer = millis();
            step_detected = true;
        }
    }


    if (readyForStep && (currentTime >= (lastStepTimer + zStepDuration)))
    {
        if (!step_detected && (z >= (zEMA + zAvgDiffThreshold)))
        {
            lastStepTimer = millis();
            step_detected = true;
        }
    }

    if (step_detected)
    {
        Serial.print("\t\t\t\tSTEP["); Serial.print(name); Serial.println("]!");
    }

    return step_detected;
}

void Leg::off()
{
    for (int i = 0; i < pixelCount; i++)
        pixels[i] = CRGB::Black;
}
