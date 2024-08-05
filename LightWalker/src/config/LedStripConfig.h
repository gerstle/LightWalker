#ifndef CONFIG_LEDSTRIPCONFIG_H_
#define CONFIG_LEDSTRIPCONFIG_H_

#include <Arduino.h>
#include "FastLED.h"
#include "../accelerometer/LWAccelerometer.h"

namespace config {

struct LedStripConfig {
	char const *name;
	CRGB *pixels;
	byte pixelCount;
	byte pixelHalf;
	int octoBaseIndex;
	lw::LWAccelerometer *accelerometer;

	LedStripConfig(char const *name, CRGB *pixels, byte pixelCount, byte pixelHalf, int octoBaseIndex) {
		this->name = name;
		this->pixels = pixels;
		this->pixelCount = pixelCount;
		this->pixelHalf = pixelHalf;
		this->octoBaseIndex = octoBaseIndex;
		this->accelerometer = NULL;
	}

	void setAccelerometer(lw::LWAccelerometer *accelerometer) {
		this->accelerometer = accelerometer;
	}
};

} /* namespace config */

#endif /* CONFIG_LEDSTRIPCONFIG_H_ */
