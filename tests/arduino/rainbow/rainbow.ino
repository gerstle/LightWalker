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

const int LEDS = 50;
RGB _pixels[LEDS];



int pixelsPerColor = 3;
int minBrightness = 0;
int maxBrightness = 255;
int increment = 5;
int 

void setup()
{
    Serial.begin(9600);
    TCL.begin();
    TCL.sendEmptyFrame();
    RGB tmpRGB;
    tmpRGB[0].r = 255;
    tmpRGB[0].g = 0;
    tmpRGB[0].b = 0;
    _pixels[0].r = 255;
    _pixels[0].g = 0;
    _pixels[0].b = 0;

    for (int i = 0; i < LEDS; i = i * pixelsPerColor)
    {
        if (rUp)
            _pixels[i].r += increment;


        for (int j = 0; j < pixelsPerColor; j++)
            TCL.sendColor(
    }
    Serial.println("init done");
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
