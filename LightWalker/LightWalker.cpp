// Do not remove the include below
#include "LightWalker.h"
#include "lw/LW.h"
#include <vector>

#define FLOATING_PIN 20

util::Bluetooth bluetooth;
LW lightwalker;

void setup() {
	delay(2000);
	Serial.begin(9600);
	Serial.println("starting");

	bluetooth.init(&lightwalker);

	// <gerstle> eq setup
	Serial.print("microphone... ");
	lightwalker.initAudio();
	Serial.println("check");

	// <cgerstle> Join i2c bus as master
	Serial.print("i2c bus...");
	Wire1.begin();
	Serial.println("check");

	Serial.println("legs...");
	lightwalker.initLegs(masterOff);
	Serial.println("    check");
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
