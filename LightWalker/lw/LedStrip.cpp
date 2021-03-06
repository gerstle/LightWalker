/*
 * LedStrip.cpp
 *
 *  Created on: Nov 8, 2018
 *      Author: cgerstle
 */

#include "LedStrip.h"
#include "../modes/OffMode.h"
#include "../modes/ChaosMode.h"
#include "../modes/EqualizerMode.h"
#include "../modes/SparkleMode.h"
#include "../modes/RainbowMode.h"
#include "../modes/ZebraMode.h"
#include "../modes/BubbleMode.h"
#include "../modes/FlamesMode.h"
#include "../modes/GravityMode.h"
#include "../config/Enums.h"

namespace lw {

LedStrip::LedStrip(config::Properties *properties, config::LedStripConfig *config) {
	this->properties = properties;
	this->config = config;

	offMode = new modes::OffMode(properties, config);
	modes[masterOff] = offMode;
	modes[chaos] = new modes::ChaosMode(properties, config);
	modes[equalizer] = new modes::EqualizerMode(properties, config);
	modes[sparkle] = new modes::SparkleMode(properties, config);
	modes[rainbow] = new modes::RainbowMode(properties, config);
	modes[zebra] = new modes::ZebraMode(properties, config);
	modes[bubble] = new modes::BubbleMode(properties, config);
	modes[flames] = new modes::FlamesMode(properties, config);
	modes[gravity] = new modes::GravityMode(properties, config);

	currentMode = offMode;
}

LedStrip::~LedStrip() {
}

void LedStrip::init() {
	Serial.print("LedStrip::init...");
	Serial.print(config->name);
	Serial.println(" initialized with:");
	Serial.print("    pixel count: ");
	Serial.println(config->pixelCount);
	Serial.print("    half: ");
	Serial.println(config->pixelHalf);
}

void LedStrip::frame() {
	currentMode->frame(false);
}

void LedStrip::setColor(CRGB color) {
	for (int i = 0; i < config->pixelCount; i++)
		config->pixels[i] = color;
}

void LedStrip::setMode(WalkingModeEnum mode) {
	std::map<WalkingModeEnum, modes::Mode*>::iterator modeIt = modes.find(mode);
	if (modeIt != modes.end()) {
		currentMode = modeIt->second;
		modeIt->second->setup();
	} else {
		currentMode = offMode;
	}
}

config::LedStripConfig* LedStrip::getConfig() {
	return config;
}

} /* namespace lw */
