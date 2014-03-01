#include "FastLED.h"

#define LED_COUNT 200
CRGB leds[LED_COUNT];

void setup()
{
    Serial.print("leds... ");
    LEDS.addLeds<P9813, 11, 13, RGB, DATA_RATE_MHZ(15)>(leds, LED_COUNT);
    LEDS.setBrightness(25);
    LEDS.showColor(CRGB::Green);
    delay(400);
    Serial.println("check");
}

void loop()
{
    CRGB newColor = CHSV(random(0, 256), 255, 255);
    LEDS.showColor(newColor);
    delay(1000);
}
