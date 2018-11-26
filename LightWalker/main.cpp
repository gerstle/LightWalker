// Do not remove the include below
#include "main.h"

#include <vector>
#include "lw/LightWalker.h"
#include "config/LWConfigs.h"

config::Properties properties;
util::Bluetooth bluetooth(&properties);
lw::LightWalker lightwalker(&properties);

void setup() {
	delay(2000);
	Serial.begin(9600);
	Serial.println("starting");

	bluetooth.init();

	// <gerstle> eq setup
	Serial.print("microphone... ");
	lightwalker.initAudio();
	Serial.println("check");

	// <cgerstle> Join i2c bus as master
	Serial.print("i2c bus...");
	Wire1.begin();
	Serial.println("check");

	Serial.println("legs...");
	lightwalker.initLegs();
	Serial.println("Walking!");
}

unsigned long statusTimer = millis();
unsigned long currentTime = millis();
int frameCounter = 0;

void loop() {
	random16_add_entropy(analogRead(FLOATING_PIN));

	currentTime = millis();
	frameCounter++;
	if (currentTime >= (statusTimer + 1000)) {
		statusTimer = currentTime;
		Serial.print("[");
		Serial.print(frameCounter);
		Serial.println("]");
		frameCounter = 0;
	}

	lightwalker.walk();
	bluetooth.listen();
}
