// Do not remove the include below
#include "LightWalker.h"
#include "lw/LW.h"
#include "audio/audio.h"

#define FLOATING_PIN 20

comms::Bluetooth bluetooth;
LW lightwalker;

void setup()
{
    delay(2000);
    Serial.begin(9600);
    Serial.println("yep starting!");

    bluetooth.init(&lightwalker);

    // <gerstle> lights setup
    Serial.print("leds... ");
    //FastLED.setDither(0);
    LEDS.addLeds<P9813, LED_CLOCK_PIN, LED_DATA_PIN, RGB, DATA_RATE_MHZ(8)>((CRGB *)(lightwalker.leds), LED_COUNT);
    LEDS.setBrightness(50);
    LEDS.showColor(CRGB::Green);
    delay(400);
    Serial.println("check");

    // <gerstle> audio setup
    Serial.print("microphone... ");
    pinMode(AUDIO_PIN, INPUT);
    audioSetup();
    Serial.println("check");

    // <cgerstle> Join i2c bus as master
    Serial.print("i2c bus...");
    Wire.begin();
    Serial.println("check");

    Serial.println("legs...");
    lightwalker.initLegs(masterOff);
    Serial.println("    check");
    Serial.println("Walking!");
}

unsigned long statusTimer = millis();
unsigned long currentTime = millis();
int frameCounter = 0;

void loop()
{
    random16_add_entropy(analogRead(FLOATING_PIN));

    currentTime = millis();
    frameCounter++;
    if (currentTime >= (statusTimer + 1000))
    {
        statusTimer = currentTime;
        Serial.print("["); Serial.print(frameCounter); Serial.println("]");
        frameCounter = 0;
    }

    lightwalker.walk();
	bluetooth.listen();
}
