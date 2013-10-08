// <gerstle> lightwalker.ino
//
// sketch for the LightWalker costume
// The main work is in the LW & Leg classes

#include <ADXL345.h>
#include <Wire.h>
#include <SPI.h>
#include <TCL.h>
#include <avr/pgmspace.h>
#include "colors.h"
#include "LWUtils.h"
#include "LW.h"

LW lw;

// <gerstle> bluetooth comms
const int msgLength = 128;
char msg[msgLength];
int msgIndex = 0;
char *pKey = NULL;
char *pValue = NULL;
char one_str[] = "1";

void setup()
{
    // <gerstle> general setup
    Serial.begin(9600);
    Serial.println("...");
    randomSeed(long(millis()));

    // <gerstle> bluetooth setup
    Serial1.begin(57600);
    while (!Serial1)
    {
        ; // <gerstle> wait for bluetooth
    }
    Serial1.print("SettingsPlease\r");

    // <gerstle> lights setup
    TCL.begin();
    TCL.setupDeveloperShield();
    TCL.sendEmptyFrame();

    // <gerstle> audio setup
    pinMode(AUDIO_PIN, INPUT);
    pinMode(AUDIO_STROBE_PIN, OUTPUT);
    pinMode(AUDIO_RESET_PIN, OUTPUT);
    analogReference(DEFAULT);

    digitalWrite(AUDIO_RESET_PIN, LOW);
    digitalWrite(AUDIO_STROBE_PIN, HIGH);

    // <cgerstle> Join i2c bus as master
    Wire.begin();

    lw.initLegs(masterOff);

    Serial.println("Walking!");
}

void loop()
{
    // <gerstle> perform LightWalker action
    lw.walk();
}

void serialEvent1()
{
    if (Serial1.available())
    {
        msg[msgIndex++] = (char)Serial1.read();

        if (msg[msgIndex - 1] == '\r')
        {
            //Serial.println(msg);
            pValue = strchr(msg, '=');
            if (pValue != NULL)
            {
                pKey = msg;
                *pValue = '\0';
                pValue++;

                executeCommand(atoi(pKey), pValue, msgIndex - (pValue - pKey) - 1);
            }

            memset(msg, '\0', msgIndex - 1);
            msgIndex = 0;
        }
    }
}

bool executeCommand(int key, char *value, int valueLen)
{
    Serial.print("key: "); Serial.print(key);
    Serial.print(" value: "); Serial.print(value); Serial.print("("); Serial.print(valueLen); Serial.println(")");

    int valueInt;

    switch (key)
    {
        // ------------------------------------------------------------------------
        // Main 
        // ------------------------------------------------------------------------
        case mainMinBrightness:
            lw.config.main.minBrightness = atoi(value);
            ResetColor();
            break;
        case mainMaxBrightness:
            lw.config.main.maxBrightness = atoi(value);
            ResetColor();
            break;
        case mainLegsOn:
            // <cgerstle> being a little lazy here... should do all this just by the configs, but...
            if (strncmp(value, one_str, valueLen) == 0)
            {
                lw.config.main.legsOn = true;
                lw.setLegsOn();
            }
            else
            {
                lw.config.main.legsOn = false;
                lw.setLegsOff();
            }
            break;
        case mainArmsOn:
            // <cgerstle> being a little lazy here... should do all this just by the configs, but...
            if (strncmp(value, one_str, valueLen) == 0)
            {
                lw.config.main.armsOn = true;
                lw.setArmsOn();
            }
            else
            {
                lw.config.main.armsOn = false;
                lw.setArmsOff();
            }
            break;

        // ------------------------------------------------------------------------
        // Mode 
        // ------------------------------------------------------------------------
        case mode:
            valueInt = atoi(value);
            WalkingModeEnum newMode;
            ResetColor();

            switch (valueInt)
            {
                case masterOff:
                    newMode = masterOff;
                    break;
                case gravity:
                    newMode = gravity;
                    break;
                case equalizer:
                    newMode = equalizer;
                    break;
                case sparkle:
                    newMode = sparkle;
                    break;
                case pulse:
                    newMode = pulse;
                    break;
                case bubble:
                    newMode = bubble;
                    break;
            }

            lw.setMode(newMode);
            break;

        // ------------------------------------------------------------------------
        // Sparkle
        // ------------------------------------------------------------------------
        case sparkleFadeRate:
            lw.config.sparkle.fadeRate = atoi(value);
            break;  
        case sparkleFlashLength:
            lw.config.sparkle.flashLength = atoi(value);
            break;
        case sparkleSparkleLength:
            lw.config.sparkle.sparkleLength = atoi(value);
            break;
        case sparkleFootFlashColor:
            ParseColor(value, &(lw.config.sparkle.footFlashColor));
            break;
        case sparkleSparkleColor:
            ParseColor(value, &(lw.config.sparkle.sparkleColor));
            break;

        // ------------------------------------------------------------------------
        // Pulse
        // ------------------------------------------------------------------------
        case pulseMinRate:
            lw.config.pulse.minPulseTime = atoi(value);
            break;
        case pulseMaxRate:
            lw.config.pulse.maxPulseTime = atoi(value);
            break;
        case pulseRandomColor:
            if (strncmp(value, one_str, valueLen) == 0)
                lw.config.pulse.randomColor = true;
            else
                lw.config.pulse.randomColor = false;
            break;

        case pulseSyncLegs:
            if (strncmp(value, one_str, valueLen) == 0)
                lw.config.pulse.syncLegs = true;
            else
                lw.config.pulse.syncLegs  = false;
            break;
        case pulseColor:
            ParseColor(value, &(lw.config.pulse.color));
            LWUtils.printRGB(lw.config.pulse.color, true);
            break;

        // ------------------------------------------------------------------------
        // Equalizer
        // ------------------------------------------------------------------------
        case eqColor:
            ParseColor(value, &(lw.config.equalizer.color));
            break;
        case eqRMSThreshold:
            lw.config.equalizer.RMSThreshold = atoi(value);
            break;
        case eqAllLights:
            if (strncmp(value, one_str, 1) == 0)
                lw.config.equalizer.allLights = true;
            else
                lw.config.equalizer.allLights = false;
            break;
        case eqAllBands:
            if (strncmp(value, one_str, 1) == 0)
                lw.config.equalizer.allBands = true;
            else
                lw.config.equalizer.allBands = false;
            break;
        case eqRainbow:
            if (strncmp(value, one_str, 1) == 0)
                lw.config.equalizer.rainbow = true;
            else
                lw.config.equalizer.rainbow = false;
            break;

        // ------------------------------------------------------------------------
        // Gravity
        // ------------------------------------------------------------------------
        case gravityRotate:
            if (strncmp(value, one_str, 1) == 0)
                lw.config.gravity.rotate = true;
            else
                lw.config.gravity.rotate = false;
            break;

        //------------------------------------------------------------------------
        // Bubble
        //------------------------------------------------------------------------
        case bubbleBackgroundColor:
            ParseColor(value, &(lw.config.bubble.backgroundColor));
            break;
        case bubbleBubbleColor:
            ParseColor(value, &(lw.config.bubble.bubbleColor));
            break;
        case bubbleSpeed:
            lw.config.bubble.speed = atoi(value);
            break;
        case bubbleWidth:
            lw.config.bubble.width = atoi(value);
            break;
        case bubbleTrail:
            if (strncmp(value, one_str, 1) == 0)
                lw.config.bubble.trail = true;
            else
                lw.config.bubble.trail = false;
            break;

        default:
            return false;
            break;
    }

    return true;
}

void ParseColor(char *colorString, RGB *color)
{
    char *value = colorString;
    char *pComma = strchr(colorString, ',');
    *pComma = '\0';

    (*color).r = byte(atoi(value));

    value = pComma + 1;
    pComma = strchr(value, ',');
    *pComma = '\0';

    (*color).g = byte(atoi(value));
    (*color).b = byte(atoi(pComma + 1));

    //LWUtils.printRGB(*color, false); Serial.print(" -> ");
    //(*color).r = map((*color).r, 0, 255, 0, lw.config.main.maxBrightness);
    //(*color).g = map((*color).g, 0, 255, 0, lw.config.main.maxBrightness);
    //(*color).b = map((*color).b, 0, 255, 0, lw.config.main.maxBrightness);
    //LWUtils.printRGB(*color, true);
}

void ResetColor()
{
    // ------------------------------------------------------------------------
    // <cgerstle> EQ stuff
    // ------------------------------------------------------------------------
    lw.config.equalizer.maxColor.r = map(lw.config.equalizer.color.r, 0, 255, 0, lw.config.main.maxBrightness);
    lw.config.equalizer.maxColor.g = map(lw.config.equalizer.color.g, 0, 255, 0, lw.config.main.maxBrightness);
    lw.config.equalizer.maxColor.b = map(lw.config.equalizer.color.b, 0, 255, 0, lw.config.main.maxBrightness);

    // <cgerstle> equalizer min
    int maximum = 0;

    if (lw.config.equalizer.color.r > 0)
        maximum = max(maximum, lw.config.equalizer.color.r);
    if (lw.config.equalizer.color.g > 0)
        maximum = max(maximum, lw.config.equalizer.color.g);
    if (lw.config.equalizer.color.b > 0)
        maximum = max(maximum, lw.config.equalizer.color.b);

    if (lw.config.equalizer.color.r == maximum)
        lw.config.equalizer.minColor.r = lw.config.main.minBrightness;
    else
        lw.config.equalizer.minColor.r = 0;

    if (lw.config.equalizer.color.g == maximum)
        lw.config.equalizer.minColor.g = lw.config.main.minBrightness;
    else
        lw.config.equalizer.minColor.g = 0;

    if (lw.config.equalizer.color.b == maximum)
        lw.config.equalizer.minColor.b = lw.config.main.minBrightness;
    else
        lw.config.equalizer.minColor.b = 0;

    // ------------------------------------------------------------------------
    // <cgerstle> Min/Max stuff
    // ------------------------------------------------------------------------
    for (int i = WHITE; i < COLOR_COUNT; i++)
    {
        lw.config.main.maxColors[i].r = map(COLORS[i].r, 0, 255, 0, lw.config.main.maxBrightness);
        lw.config.main.maxColors[i].g = map(COLORS[i].g, 0, 255, 0, lw.config.main.maxBrightness);
        lw.config.main.maxColors[i].b = map(COLORS[i].b, 0, 255, 0, lw.config.main.maxBrightness);

        lw.config.main.minColors[i].r = map(COLORS[i].r, 0, 255, 0, lw.config.main.minBrightness);
        lw.config.main.minColors[i].g = map(COLORS[i].g, 0, 255, 0, lw.config.main.minBrightness);
        lw.config.main.minColors[i].b = map(COLORS[i].b, 0, 255, 0, lw.config.main.minBrightness);
    }
}
