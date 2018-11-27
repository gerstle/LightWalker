#include "LightWalker.h"

#include <OctoWS2811.h>
#include "FastLED.h"
#include "AccelerometerLedStrip.h"
#include "../config/LedStripConfig.h"

namespace lw {

DMAMEM int displayMemory[LEDS_PER_STRIP * 6];
int drawingMemory[LEDS_PER_STRIP * 6];
OctoWS2811 octoLeds(LEDS_PER_STRIP, displayMemory, drawingMemory,
		WS2811_RGB | WS2811_800kHz);

LightWalker::LightWalker(config::Properties *properties) {
	this->properties = properties;
	currentMode = masterOff;
	_pulse_length = 0;
}

void LightWalker::initLegs() {
	Serial.println("creating LED strips");
	ledStrips.push_back(
			new AccelerometerLedStrip(properties,
					new config::LedStripConfig("left leg",
							new CRGB[LEG_PIXEL_COUNT], LEG_PIXEL_COUNT,
							LEG_HALF, 100), ADXL_LEFT_LEG));
	ledStrips.push_back(
			new AccelerometerLedStrip(properties,
					new config::LedStripConfig("right leg",
							new CRGB[LEG_PIXEL_COUNT], LEG_PIXEL_COUNT,
							LEG_HALF, 300), ADXL_RIGHT_LEG));
	ledStrips.push_back(
			new AccelerometerLedStrip(properties,
					new config::LedStripConfig("left arm",
							new CRGB[ARM_PIXEL_COUNT], ARM_PIXEL_COUNT,
							ARM_HALF, 0), ADXL_LEFT_ARM));
	ledStrips.push_back(
			new AccelerometerLedStrip(properties,
					new config::LedStripConfig("right arm",
							new CRGB[ARM_PIXEL_COUNT], ARM_PIXEL_COUNT,
							ARM_HALF, 200), ADXL_RIGHT_ARM));
	ledStrips.push_back(
			new LedStrip(properties,
					new config::LedStripConfig("horns",
							new CRGB[HEAD_PIXEL_COUNT], HEAD_PIXEL_COUNT,
							HEAD_PIXEL_COUNT / 2, 400)));

	Serial.println("initializing LED strips");
	octoLeds.begin();
	for (std::vector<LedStrip*>::iterator stripIt = ledStrips.begin(); stripIt != ledStrips.end(); ++stripIt) {
		(*stripIt)->init();
		testLeg(*stripIt);
	}

	LEDS.show();
	delay(1000);
}

void LightWalker::testLeg(LedStrip *strip) {
	random16_add_entropy(analogRead(FLOATING_PIN));
	byte h = random8();
	strip->setColor(CHSV(h, 255, 255));
	transferToOcto(strip);
	octoLeds.show();
	delay(200);
}

void LightWalker::initAudio() {
	eq.init();
}

void LightWalker::setMode(WalkingModeEnum m) {
	currentMode = m;
	for (std::vector<LedStrip*>::iterator stripIt = ledStrips.begin(); stripIt != ledStrips.end(); ++stripIt)
		(*stripIt)->setMode(currentMode);
}

void LightWalker::transferToOcto(LedStrip *strip) {
	config::LedStripConfig *config = strip->getConfig();
	for (int j = 0; j < config->pixelCount; j++)
		octoLeds.setPixel(config->octoBaseIndex + j, config->pixels[j].r,
				config->pixels[j].g, config->pixels[j].b);
}

// <gerstle> check for inputs
char eqStringVal[12];
void LightWalker::walk() {
	// <gerstle> only listen to the EQ once per frame
	if (currentMode == equalizer) {
		snprintf (eqStringVal, sizeof(eqStringVal), "%f", eq.listen());
		properties->set(eqLevel, eqStringVal);
	}

	WalkingModeEnum mode = static_cast<WalkingModeEnum>(properties->getInt(Preferences::mode, (int)currentMode));
	if (currentMode != mode)
		Serial.printf("transitioning to mode %d\n", mode);

	for (std::vector<LedStrip*>::iterator stripIt = ledStrips.begin(); stripIt != ledStrips.end(); ++stripIt) {
		if (currentMode != mode) {
			(*stripIt)->setMode(mode);
		}

		// <cgerstle> paint the lights.
		(*stripIt)->frame();
		transferToOcto(*stripIt);
	}
	currentMode = mode;

	// <cgerstle> sends the colors out to the lights
	octoLeds.show();
}

} /* namespace lw */
