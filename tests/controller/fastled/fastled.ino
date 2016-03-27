#include "FastLED.h"

#define LED_COUNT 3
CRGB leds[LED_COUNT];

void setup()
{
    Serial.print("leds... ");
    // left arm
    //LEDS.addLeds<P9813, 8, 9, RGB, DATA_RATE_MHZ(15)>(leds, LED_COUNT);
    // horns
    //LEDS.addLeds<P9813, 4, 5, RGB, DATA_RATE_MHZ(15)>(leds, LED_COUNT);
    // right arm
    //LEDS.addLeds<P9813, 6, 7, RGB, DATA_RATE_MHZ(15)>(leds, LED_COUNT);
    // legs
    LEDS.addLeds<P9813, 10, 11, RGB, DATA_RATE_MHZ(15)>(leds, LED_COUNT);

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
