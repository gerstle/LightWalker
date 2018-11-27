#include "OffMode.h"

namespace modes {

OffMode::OffMode(config::Properties *properties, config::LedStripConfig *config) : Mode(properties, config) {
}

OffMode::~OffMode() {
}

void OffMode::frame(bool stepDetected) {
	for (int i = 0; i < config->pixelCount; i++)
		config->pixels[i] = CRGB::Black;
}

const char* OffMode::getName() {
	return "off";
}

} /* namespace config */
