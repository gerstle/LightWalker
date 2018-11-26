/*
 * LWAccelerometer.h
 *
 *  Created on: Nov 8, 2018
 *      Author: cgerstle
 */

#ifndef ACCELEROMETER_LWACCELEROMETER_H_
#define ACCELEROMETER_LWACCELEROMETER_H_

#include "ADXL345_compatible.h"
#include "../config/LWConfigs.h"
#include <i2c_t3.h>

namespace lw {

class LWAccelerometer: public ADXL345 {
public:
	LWAccelerometer(byte channel);
	virtual ~LWAccelerometer();
	void init();
	void readAccel(short *xyz);
	void readXYZ(short *x, short *y, short *z);
private:
	void selectChannel();
	byte i2cChannel;
};

} /* namespace lw */

#endif /* ACCELEROMETER_LWACCELEROMETER_H_ */
