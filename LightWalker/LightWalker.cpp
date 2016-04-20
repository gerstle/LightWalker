// Do not remove the include below
#include "LightWalker.h"
#include "lw/LW.h"

#define FLOATING_PIN 20

util::Bluetooth bluetooth;
LW lightwalker;

void setup()
{
    delay(2000);
    Serial.begin(9600);
    Serial.println("yep starting");

    bluetooth.init(&lightwalker);

    // <gerstle> lights setup
    Serial.print("leds... ");
    // legs
    LEDS.addLeds<P9813, 10, 11, RGB, DATA_RATE_MHZ(8)>(lightwalker.leds_legs, LED_COUNT);
    // left arm
    LEDS.addLeds<P9813, 8, 9, RGB, DATA_RATE_MHZ(8)>(lightwalker.leds_left_arm, LED_COUNT);
    // right arm
    LEDS.addLeds<P9813, 6, 7, RGB, DATA_RATE_MHZ(8)>(lightwalker.leds_right_arm, LED_COUNT);
    // horns
    LEDS.addLeds<P9813, 4, 5, RGB, DATA_RATE_MHZ(8)>(lightwalker.leds_horns, LED_COUNT);

    LEDS.setBrightness(50);
    LEDS.showColor(CRGB::Green);
    delay(400);
    Serial.println("check");

    // <gerstle> eq setup
    Serial.print("microphone... ");
    lightwalker.initAudio();
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
