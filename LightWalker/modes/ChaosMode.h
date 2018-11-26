/*
 * ChaosMode.h
 *
 *  Created on: Nov 8, 2018
 *      Author: cgerstle
 */

#ifndef MODESCHAOSMODEH
#define MODESCHAOSMODEH

#include "Mode.h"
#include "../config/Properties.h"
#include "../config/LedStripConfig.h"

namespace modes {

class ChaosMode: public modes::Mode {
public:
	ChaosMode(config::Properties *properties, config::LedStripConfig *config);
	virtual ~ChaosMode();

	void frame(bool stepDetected);
	const char* getName();

private:
	config::Properties *properties;
	double perlinsTracker;
	double x;
	double y;
	byte hue;
	int direction;
	int directionTimer;
	unsigned long lastStepTime;
};

} /* namespace mode */

#endif /* MODESCHAOSMODEH */
