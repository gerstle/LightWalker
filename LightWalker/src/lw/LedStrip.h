/*
 * LedStrip.h
 *
 *  Created on: Nov 8, 2018
 *      Author: cgerstle
 */

#ifndef LW_LEDSTRIP_H_
#define LW_LEDSTRIP_H_

#include "LWUtils.h"
#include "../config/LedStripConfig.h"
#include "FastLED.h"
#include <map>

#include "../modes/Mode.h"
#include "../config/Properties.h"

namespace lw {

class LedStrip {
public:
	LedStrip(config::Properties *properties, config::LedStripConfig *config);
	virtual ~LedStrip();
	virtual void init();
	virtual void frame();
	void off();
	void setMode(WalkingModeEnum mode);
	void setColor(CRGB color);
	config::LedStripConfig* getConfig();
protected:
	config::Properties *properties;
	std::map<WalkingModeEnum, modes::Mode*> modes;
	modes::Mode *currentMode;
	config::LedStripConfig *config;
	modes::Mode *offMode;
};

} /* namespace lw */

#endif /* LW_LEDSTRIP_H_ */
