/*****************************************************************************
 * rainbow.ino
 *
 * This example will send a sequence of rainbow colors down a 25 pixel long
 * strand of total control lighting. The Arduino will send an update every
 * second to the strand, although this is unnecessary and if you disconnect
 * the Arduino from the strand, you will see that as long as the lights have
 * power, they will retain the last color information sent to them.
 *
 * Copyright 2011 Christopher De Vries
 * This program is distributed under the Artistic License 2.0, a copy of which
 * is included in the file LICENSE.txt along with this library.
 ****************************************************************************/
#include <SPI.h>
#include <TCL.h>
#include "colors.h"

const int LEDS = 100;
RGB _pixels[LEDS];

const int RAINBOW_COUNT = 33;
const RGB RAINBOW[RAINBOW_COUNT] =
{
    RGB(0xff, 0x00, 0x00),
    RGB(0xff, 0x00, 0x00),
    RGB(0xff, 0x1f, 0x00),
    RGB(0xff, 0x3e, 0x00),
    RGB(0xff, 0x7d, 0x00),
    RGB(0xff, 0xff, 0x00),
    RGB(0xff, 0xff, 0x00),
    RGB(0xbb, 0xff, 0x00),
    RGB(0x7d, 0xff, 0x00),
    RGB(0x3e, 0xff, 0x00),
    RGB(0x00, 0xff, 0x00),
    RGB(0x00, 0xff, 0x00),
    RGB(0x00, 0xff, 0x3e),
    RGB(0x00, 0xff, 0x7d),
    RGB(0x00, 0xff, 0xbb),
    RGB(0x00, 0xff, 0xff),
    RGB(0x00, 0xff, 0xff),
    RGB(0x00, 0xbb, 0xff),
    RGB(0x00, 0x7d, 0xff),
    RGB(0x00, 0x3e, 0xff),
    RGB(0x00, 0x00, 0xff),
    RGB(0x00, 0x00, 0xff),
    RGB(0x1f, 0x00, 0xff),
    RGB(0x3e, 0x00, 0xff),
    RGB(0x7d, 0x00, 0xff),
    RGB(0x88, 0x00, 0xff),
    RGB(0xff, 0x00, 0xff),
    RGB(0xff, 0x00, 0xff),
    RGB(0xff, 0x00, 0x88),
    RGB(0xff, 0x00, 0x7d),
    RGB(0xff, 0x00, 0x3e),
    RGB(0xff, 0x00, 0x1f),
    RGB(0xff, 0x00, 0x0f),
};
// const RGB RAINBOW[RAINBOW_COUNT] =
// {
//     RGB(0xff, 0x00, 0x00),
//     RGB(0xff, 0x00, 0x00),
//     RGB(0xff, 0x1f, 0x00),
//     RGB(0xff, 0x3e, 0x00),
//     RGB(0xff, 0x7d, 0x00),
//     RGB(0xff, 0xff, 0x00),
//     RGB(0xbb, 0xff, 0x00),
//     RGB(0x7d, 0xff, 0x00),
//     RGB(0x3e, 0xff, 0x00),
//     RGB(0x00, 0xff, 0x00),
//     RGB(0x00, 0xff, 0x00),
//     RGB(0x00, 0xff, 0x00),
//     RGB(0x00, 0xff, 0x3e),
//     RGB(0x00, 0xff, 0x7d),
//     RGB(0x00, 0xff, 0xff),
//     RGB(0x00, 0xbb, 0xff),
//     RGB(0x00, 0x7d, 0xff),
//     RGB(0x00, 0x3e, 0xff),
//     RGB(0x00, 0x00, 0xff),
//     RGB(0x00, 0x00, 0xff),
//     RGB(0x1f, 0x00, 0xff),
//     RGB(0x7d, 0x00, 0xff),
//     RGB(0xff, 0x00, 0xff),
//     RGB(0xff, 0x00, 0x7d),
//     RGB(0xff, 0x00, 0x1f)
// };

int pixelsPerColor = 6;
int rainbowIndex = 0;
int rainbowTotal = RAINBOW_COUNT * pixelsPerColor;
int minBrightness = 0;
int maxBrightness = 255;

void setup()
{
    Serial.begin(9600);
    TCL.begin();
    TCL.sendEmptyFrame();

    int color = 0;
    int pixelCount = 0;

    print("rainbowTotal", rainbowTotal);
    print("pixelsPerColor", pixelsPerColor);
    print("rainbow index", rainbowIndex);

    for (int i = rainbowIndex; ((i < RAINBOW_COUNT) && (pixelCount < LEDS)); i++)
    {
        print("rainbow index", rainbowIndex);
        print("color", color);
        if (color >= RAINBOW_COUNT)
            color = 0;

        for (int j = 0; j < pixelsPerColor; j++, rainbowIndex++)
        {
            TCL.sendColor(RAINBOW[color].r, RAINBOW[color].g, RAINBOW[color].b);
            pixelCount++;
        }
        color++;
    }
    Serial.println("init done");
    print("rainbow index", rainbowIndex);
}

bool stopped = false;
int lightDelay = 50;
unsigned long lastChangeTime = millis();
unsigned long currentTime = millis();
void loop()
{
    currentTime = millis();
    if (currentTime <= (lastChangeTime + lightDelay))
        return;

    lastChangeTime = millis();
        
    TCL.sendEmptyFrame();
    int rainbowSubIndex = rainbowIndex % pixelsPerColor;
    int color = rainbowIndex / pixelsPerColor;
    int pixelCount = 0;
    int i;
    bool firstPixel = true;
    for (i = rainbowIndex; ((i < rainbowTotal) && (pixelCount < LEDS)); i++)
    {
        if (color >= RAINBOW_COUNT)
            color = 0;

        for (int j = rainbowSubIndex; ((j < pixelsPerColor) && (pixelCount < LEDS)); j++)
        {
            if (firstPixel)
            {
                printRGB(RAINBOW[color], true);
                firstPixel = false;
            }
            TCL.sendColor(RAINBOW[color].r, RAINBOW[color].g, RAINBOW[color].b);
            pixelCount++;
        }
        color++;
        rainbowSubIndex = 0;
    }
    rainbowIndex++;

    if (rainbowIndex >= rainbowTotal)
        rainbowIndex = 0;
}

void print(char *key, char *value)
{
    Serial.print(key);
    Serial.print(": ");
    Serial.println(value);
}

void print(char *key, int value)
{
    Serial.print(key);
    Serial.print(": ");
    Serial.println(value);
}

void printRGB(RGB color, bool newLine)
{
    printRGB(color.r, color.g, color.b, newLine);
}

void printRGB(byte r, byte g, byte b, bool newLine)
{
    Serial.print(r); Serial.print(", "); Serial.print(g); Serial.print(", "); Serial.print(b);

    if (newLine)
        Serial.println();
}

// void _setPixel(int i, RGB color, byte dimmer)
// {
//     if (color.r > dimmer)
//         color.r = color.r - dimmer;
//     else
//         color.r = 0;
// 
//     if (color.g > dimmer)
//         color.g = color.g - dimmer;
//     else
//         color.g = 0;
// 
//     if (color.b > dimmer)
//         color.b = color.b - dimmer;
//     else
//         color.b = 0;
// 
//     if (color.r > 0)
//         _pixels[i].r = map(color.r, minBrightness, 255, minBrightness, maxBrightness);
//     else
//         _pixels[i].r = 0;
// 
//     if (color.g > 0)
//         _pixels[i].g = map(color.g, minBrightness, 255, minBrightness, maxBrightness);
//     else
//         _pixels[i].g = 0;
// 
//     if (color.b > 0)
//         _pixels[i].b = map(color.b, minBrightness, 255, minBrightness, maxBrightness);
//     else
//         _pixels[i].b = 0;
// 
//     TCL.sendColor(_pixels[i].r, _pixels[i].g, _pixels[i].b);
// }
