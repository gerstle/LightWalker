/*
 * LWAccelerometer.cpp
 *
 *  Created on: Nov 8, 2018
 *      Author: cgerstle
 */

#include "LWAccelerometer.h"

namespace lw {

LWAccelerometer::LWAccelerometer(byte channel) {
	i2cChannel = channel;
}

LWAccelerometer::~LWAccelerometer() {
}

void LWAccelerometer::init() {

	selectChannel();
	powerOn();
	setRangeSetting(2);

	//set activity/ inactivity thresholds (0-255)
	setActivityThreshold(75); //62.5mg per increment
	setInactivityThreshold(75); //62.5mg per increment
	setTimeInactivity(10); // how many seconds of no activity is inactive?

	//look of activity movement on this axes - 1 == on; 0 == off
	setActivityX(1);
	setActivityY(1);
	setActivityZ(1);

	//set values for what is considered freefall (0-255)
	setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
	setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
}

void LWAccelerometer::selectChannel() {
	Wire1.beginTransmission(I2C_MULTIPLEXER);
	Wire1.write(i2cChannel);
	Wire1.endTransmission();
}

void LWAccelerometer::readAccel(short *xyz){
	selectChannel();
	ADXL345::readAccel(xyz);
}

void LWAccelerometer::readXYZ(short *x, short *y, short *z) {
	selectChannel();
	ADXL345::readXYZ(x, y, z);
}

} /* namespace lw */
