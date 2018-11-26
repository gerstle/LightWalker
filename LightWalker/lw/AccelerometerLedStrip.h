/*
 * AccelerometerLedStrip.h
 *
 *  Created on: Nov 8, 2018
 *      Author: cgerstle
 */

#ifndef LW_ACCELEROMETERLEDSTRIP_H_
#define LW_ACCELEROMETERLEDSTRIP_H_

#define ADXL_VALUE_COUNT 100

#include "LedStrip.h"
#include "../accelerometer/LWAccelerometer.h"

namespace lw {

class AccelerometerLedStrip: public LedStrip {
public:
	AccelerometerLedStrip(config::Properties *properties, config::LedStripConfig *config, byte channel);
	virtual ~AccelerometerLedStrip();
	void init();
	void frame();
private:
	bool detectStep();
	LWAccelerometer accelerometer;
	unsigned long lastStepTimer;
	float xEMA;
	float yEMA;
	float zEMA;

	int xNMinus1;
	int xNMinus2;

	int yNMinus1;
	int yNMinus2;

	int zNMinus1;
	int zNMinus2;

	int xStepDuration; // <cgerstle> a step lasts at least this long... ie, two steps can't occur within this time period
	int yStepDuration;
	int zStepDuration;
	int xAvgDiffThreshold;
	int yAvgDiffThreshold;
	int zAvgDiffThreshold;

	int xSignificantlyLowerThanAverageThreshold;
	bool readyForStep;
};

} /* namespace lw */

#endif /* LW_ACCELEROMETERLEDSTRIP_H_ */
