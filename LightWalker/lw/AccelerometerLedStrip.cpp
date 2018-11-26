/*
 * AccelerometerLedStrip.cpp
 *
 *  Created on: Nov 8, 2018
 *      Author: cgerstle
 */

#include "AccelerometerLedStrip.h"

namespace lw {

AccelerometerLedStrip::AccelerometerLedStrip(config::Properties *properties, config::LedStripConfig *configs, byte channel) :
		LedStrip(properties, configs), accelerometer(channel) {
	lastStepTimer = millis();
	xStepDuration = 150;
	yStepDuration = 350;
	zStepDuration = 350;
	xAvgDiffThreshold = 170;
	yAvgDiffThreshold = 150;
	zAvgDiffThreshold = 150;
	xSignificantlyLowerThanAverageThreshold = 45;
	readyForStep = true;

	xEMA = yEMA = zEMA = 0;

	xNMinus1 = xNMinus2 = yNMinus1 = yNMinus2 = zNMinus1 = zNMinus2 = 0;
}

AccelerometerLedStrip::~AccelerometerLedStrip() {
}

void AccelerometerLedStrip::init() {
	Serial.printf("AccelerometerLedStrip init... ");
	LedStrip::init();

	Serial.println("    initializing accelerometer");
	accelerometer.init();

	// <gerstle> init ADXL EMA's
	short x, y, z;
	int valueIndex = 0;
	double xValueTotal = 0;
	double yValueTotal = 0;
	double zValueTotal = 0;
	for (valueIndex = 0; valueIndex < ADXL_VALUE_COUNT; valueIndex++) {
		accelerometer.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

		// x = x;
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
	Serial.println("    done.");
}

void AccelerometerLedStrip::frame()
{
	currentMode->frame(detectStep());
}

bool AccelerometerLedStrip::detectStep()
{
    short x, y, z;
    bool step_detected = false;
    unsigned long currentTime = millis();

    //LWUtils.selectI2CChannels(_channel);
    accelerometer.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

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
        Serial.printf("\t\t\t\tSTEP[%s]!\r\n", config->name);
    }

    return step_detected;
}

} /* namespace lw */
