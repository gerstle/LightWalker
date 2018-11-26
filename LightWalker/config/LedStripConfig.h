/*
 * LedStripConfig.h
 *
 *  Created on: Nov 9, 2018
 *      Author: cgerstle
 */

#ifndef CONFIG_LEDSTRIPCONFIG_H_
#define CONFIG_LEDSTRIPCONFIG_H_

#include <Arduino.h>
#include "FastLED.h"

namespace config {

struct LedStripConfig {
	char const *name;
	CRGB *pixels;
	byte pixelCount;
	byte pixelHalf;
	int octoBaseIndex;

	LedStripConfig(char const *name, CRGB *pixels, byte pixelCount, byte pixelHalf, int octoBaseIndex) {
		this->name = name;
		this->pixels = pixels;
		this->pixelCount = pixelCount;
		this->pixelHalf = pixelHalf;
		this->octoBaseIndex = octoBaseIndex;
	}
};

} /* namespace config */

#endif /* CONFIG_LEDSTRIPCONFIG_H_ */
