/*****************************************************************************
 * LW.h
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#ifndef LW_h
#define LW_h

#include "../accelerometer/LWAccelerometer.h"
#include "LWUtils.h"
#include "../config/LWConfigs.h"
#include "../util/EQ.h"
#include "Leg.h"
#include <vector>
#include "LedStrip.h"
#include "../config/Properties.h"

namespace lw {

class LightWalker {
public:
	LightWalker(config::Properties *properties);
	void initLegs();
	void initAudio();
	void walk();
	void setMode(WalkingModeEnum m);

private:
	void testLeg(LedStrip *strip);
	void transferToOcto(LedStrip *strip);

	std::vector<LedStrip*> ledStrips;
	WalkingModeEnum currentMode;
	int _pulse_length;
	util::EQ eq;
	config::Properties *properties;
};

} /* namespace lw */

#endif
