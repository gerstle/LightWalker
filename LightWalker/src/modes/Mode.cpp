#include "../modes/Mode.h"

namespace modes {

Mode::Mode(config::Properties *properties, config::LedStripConfig *config) {
	this->properties = properties;
	this->config = config;
	stepDetected = false;
	lastChangeTimer = millis();
}

Mode::~Mode() {
}

void Mode::setup() {
}

void Mode::frame(bool stepDetected) {
	Serial.println("Base mode frame called, please implement in actual mode");
}

const char* Mode::getName() {
	return "BaseMode";
}

} /* namespace mode */
