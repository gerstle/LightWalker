#include "ChaosMode.h"
#include <FastLED.h>

namespace modes {

ChaosMode::ChaosMode(config::Properties *properties, config::LedStripConfig *config) : Mode(properties, config) {
	this->properties = properties;
	directionTimer = random16(100, 5000);
	direction = -1;
	perlinsTracker = random16();
	x = random16();
	y = random16();
	hue = 0;
	lastStepTime = millis();
}

ChaosMode::~ChaosMode() {
}

const char* ChaosMode::getName() {
	return "chaos";
}

void ChaosMode::frame(bool stepDetected) {
	unsigned long currentTime = millis();

	if (stepDetected)
		lastStepTime = currentTime;

	int stepLength = properties->getInt(chaosStepLength, 1250);
	int minValue = properties->getInt(chaosMinValue, 100);
	int speed = properties->getInt(chaosSpeed, 8);
	bool sparse = properties->getBool(chaosSparse, false);
	CHSV color = CHSV(18, 255, 255);
	properties->getCHSV(chaosColor, &color);
	int swing = properties->getInt(chaosSwing, 40);

	if (currentTime <= (lastStepTime + stepLength)) {
		hue += 32;
		byte value = map(currentTime, lastStepTime,
				lastStepTime + stepLength, 255,
				min(255, minValue + minValue / 2));
		for (int i = 0; i < config->pixelCount; i++)
			config->pixels[i].setHSV(hue, 255, value);
	} else {
		if (currentTime > (lastChangeTimer + directionTimer)) {
			lastChangeTimer = currentTime;
			directionTimer = random16(100, 1500);
			int multiplier = random8(1, 4);
			if (random8(2) == 0)
				direction = -1 * multiplier;
			else
				direction = 1 * multiplier;
		}

		x += (speed * direction);
		y += (speed * direction);

		for (int i = 0; i < config->pixelCount; i++) {
			perlinsTracker += 0.0005;
			//double adjuster = LWUtils.perlinNoise(perlinsTracker + sin(i /  4), cos(perlinsTracker), perlinsTracker);
			double adjuster = inoise8(x + i * 20, y + i * 10, perlinsTracker);

			if (sparse) {
				// <cgerstle> this basically takes it to -1, 0, or 1 and creates a cool effect
				adjuster = map(adjuster, 0, 255, -1, 1.0);
			} else
				adjuster = (adjuster - 128) / (double) 128;

			config->pixels[i].setHSV(
					color.h + (adjuster * (double) swing),
					color.s, (adjuster * (double) 127) + 128);
		}
	}
}

} /* namespace mode */
