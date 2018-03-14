#include "FastLED.h"

#define LED_COUNT 50
CRGB leds[4][LED_COUNT];

byte colors[4] = {0, 96, 64, 160};
byte bars[4] = {0, 10, 15, 20};
byte speed[4] = {1, 2, 1, 2};
byte length = 10;

void setup()
{
    Serial.print("leds... ");
    LEDS.addLeds<P9813, 7, 13, RGB, DATA_RATE_MHZ(15)>(leds[0], LED_COUNT);
    LEDS.addLeds<P9813, 7, 14, RGB, DATA_RATE_MHZ(15)>(leds[1], LED_COUNT);
    LEDS.addLeds<P9813, 11, 13, RGB, DATA_RATE_MHZ(15)>(leds[2], LED_COUNT);
    LEDS.addLeds<P9813, 11, 14, RGB, DATA_RATE_MHZ(15)>(leds[3], LED_COUNT);

    LEDS.setBrightness(75);
    LEDS.showColor(CRGB::Green);
    delay(400);
    Serial.println("check");
    LEDS.showColor(CRGB::Black);
}

unsigned long lastChange = millis();
unsigned long statusTimer = millis();
unsigned long currentTime = millis();
int frameCounter = 0;
void loop()
{

    currentTime = millis();
    frameCounter++;
    if (currentTime >= (statusTimer + 1000))
    {
        statusTimer = currentTime;
        Serial.print("["); Serial.print(frameCounter); Serial.println("]");
        frameCounter = 0;
    }
    
    
    if (currentTime >= (lastChange + 10000))
    {
        lastChange = currentTime;
        for (int i = 0; i < 4; i++)
            colors[i] += 64;
    }

    if (currentTime % 100 == 0)
        for (int i = 0; i < 4; i++)
            draw(i);

    LEDS.show();

}

void draw(byte index)
{
    Serial.print("\t"); Serial.print(colors[index]);
    for (int i = 0; i < LED_COUNT; i++)
    {
        if (i >= bars[index] && i < bars[index] + length)
            leds[index][i] = CHSV(colors[index], 255, 255);
        else
            leds[index][i] = CHSV(colors[index] + 128, 255, 128);
    }

    bars[index] += speed[index];
    if (bars[index] > LED_COUNT)
        bars[index] = 0;
}
