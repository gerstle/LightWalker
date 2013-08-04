// <gerstle> lightwalker.ino
//
// sketch for the LightWalker costume
// The main work is in the LW & Leg classes

#include <ADXL345.h>
#include <Wire.h>
#include <SPI.h>
#include <TCL.h>
#include <avr/pgmspace.h>
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

    // <gerstle> check for bluetooth data
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

                executeCommand(pKey, (pValue - pKey - 1), pValue, msgIndex - (pValue - pKey) - 1);
            }

            memset(msg, '\0', msgIndex - 1);
            msgIndex = 0;
        }
    }
}

bool executeCommand(char *key, int keyLen, char *value, int valueLen)
{
    Serial.print("key: "); Serial.print(key); Serial.print("("); Serial.print(keyLen); Serial.print(")");
    Serial.print(" value: "); Serial.print(value); Serial.print("("); Serial.print(valueLen); Serial.println(")");

    int valueInt, offset;

    // ------------------------------------------------------------------------
    // Main 
    // ------------------------------------------------------------------------
    offset = 4; 
    if (strncmp_P(key, PSTR("main"), offset) == 0)
    {
        if (strncmp_P(key + offset, PSTR("MinBrightness"), keyLen - offset) == 0)
        {
            lw.config.main.minBrightness = atoi(value);
            ResetMinValues();
        }
        if (strncmp_P(key + offset, PSTR("MaxBrightness"), keyLen - offset) == 0)
            lw.config.main.maxBrightness = atoi(value);
        else if (strncmp_P(key + offset, PSTR("LegsOn"), keyLen - offset) == 0)
            if (strncmp(value, one_str, valueLen) == 0)
                lw.config.main.legsOn = true;
            else
                lw.config.main.legsOn = false;
        else if (strncmp_P(key + offset, PSTR("ArmsOn"), keyLen - offset) == 0)
            if (strncmp(value, one_str, valueLen) == 0)
                lw.config.main.armsOn = true;
            else
                lw.config.main.armsOn = false;

        return true;
    }


    // ------------------------------------------------------------------------
    // Mode 
    // ------------------------------------------------------------------------
    offset = 4;
    if (strncmp_P(key, PSTR("mode"), offset) == 0)
    {
        WalkingModeEnum mode;
        if (strncmp_P(value, PSTR("masterOff"), valueLen) == 0)
            mode = masterOff;
        else if (strncmp_P(value, PSTR("gravity"), valueLen) == 0)
            mode = gravity;
        else if (strncmp_P(value, PSTR("equalizer"), valueLen) == 0)
            mode = equalizer;
        else if (strncmp_P(value, PSTR("sparkle"), valueLen) == 0)
            mode = sparkle;
        else if (strncmp_P(value, PSTR("pulse"), valueLen) == 0)
            mode = pulse;
        
        lw.setMode(mode);
        return true;
    }

    // ------------------------------------------------------------------------
    // Sparkle
    // ------------------------------------------------------------------------
    offset = 7;
    if (strncmp_P(key, PSTR("sparkle"), offset) == 0)
    {
        if (strncmp_P(key + offset, PSTR("FootUpFadeRate"), keyLen - offset) == 0)
            lw.config.sparkle.footUpFadeRate = atoi(value);
        else if (strncmp_P(key + offset, PSTR("FootDownFadeRate"), keyLen - offset) == 0)
            lw.config.sparkle.footDownFadeRate = atoi(value);
        else if (strncmp_P(key + offset, PSTR("FlashLength"), keyLen - offset) == 0)
            lw.config.sparkle.flashLength = atoi(value);
        else if (strncmp_P(key + offset, PSTR("SparkleLength"), keyLen - offset) == 0)
            lw.config.sparkle.sparkleLength = atoi(value);
        else if (strncmp_P(key + offset, PSTR("FootFlashColor"), keyLen - offset) == 0)
            ParseColor(value, &(lw.config.sparkle.footFlashColor));
        else if (strncmp_P(key + offset, PSTR("FootSparkleColor"), keyLen - offset) == 0)
            ParseColor(value, &(lw.config.sparkle.footSparkleColor));
        else if (strncmp_P(key + offset, PSTR("LegSparkleColor"), keyLen - offset) == 0)
            ParseColor(value, &(lw.config.sparkle.legSparkleColor));

        return true;
    }

    // ------------------------------------------------------------------------
    // PULSE
    // ------------------------------------------------------------------------
    offset = 5;
    if (strncmp_P(key, PSTR("pulse"), offset) == 0)
    {
        if (strncmp_P(key + offset, PSTR("MinRate"), keyLen - offset) == 0)
            lw.config.pulse.minPulseTime = atoi(value);
        else if (strncmp_P(key + offset, PSTR("MaxRate"), keyLen - offset) == 0)
            lw.config.pulse.maxPulseTime = atoi(value);
        else if (strncmp_P(key + offset, PSTR("Random"), keyLen - offset) == 0)
            if (strncmp(value, one_str, valueLen) == 0)
                lw.config.pulse.randomColor = true;
            else
                lw.config.pulse.randomColor = false;
        else if (strncmp_P((key + offset), PSTR("SyncLegs"), keyLen - offset) == 0)
        {
            if (strncmp(value, one_str, valueLen) == 0)
                lw.config.pulse.syncLegs = true;
            else
                lw.config.pulse.syncLegs  = false;
        }
        else if (strncmp_P(key + offset, PSTR("Color"), keyLen - offset) == 0)
            ParseColor(value, &(lw.config.pulse.color));

        return true;
    }

    // ------------------------------------------------------------------------
    // EQUALIZER
    // ------------------------------------------------------------------------
    offset = 2;
    if (strncmp_P(key, PSTR("eq"), offset) == 0)
    {
        if (strncmp_P(key + offset, PSTR("Color"), keyLen - offset) == 0)
        {
            ParseColor(value, &(lw.config.equalizer.color));
            ResetMinValues();
        }
        else if (strncmp_P(key + offset, PSTR("RMSThreshold"), keyLen - offset) == 0)
            lw.config.equalizer.RMSThreshold = atoi(value);
        else if (strncmp_P(key + offset, PSTR("AllLights"), keyLen - offset) == 0)
            if (strncmp(value, one_str, 1) == 0)
                lw.config.equalizer.allLights = true;
            else
                lw.config.equalizer.allLights = false;
        else if (strncmp_P(key + offset, PSTR("AllBands"), keyLen - offset) == 0)
            if (strncmp(value, one_str, 1) == 0)
                lw.config.equalizer.allBands = true;
            else
                lw.config.equalizer.allBands = false;

        return true;
    }

    // ------------------------------------------------------------------------
    // Gravity
    // ------------------------------------------------------------------------
    offset = 7;
    if (strncmp_P(key, PSTR("gravity"), offset) == 0)
    {
        if (strncmp_P(key + offset, PSTR("ColorOne"), keyLen - offset) == 0)
            ParseColor(value, &(lw.config.gravity.colorOne));
        else if (strncmp_P(key + offset, PSTR("ColorTwo"), keyLen - offset) == 0)
            ParseColor(value, &(lw.config.gravity.colorTwo));
        else if (strncmp_P(key + offset, PSTR("ColorThree"), keyLen - offset) == 0)
            ParseColor(value, &(lw.config.gravity.colorThree));
        else if (strncmp_P(key + offset, PSTR("Rotate"), keyLen - offset) == 0)
            if (strncmp(value, one_str, 1) == 0)
                lw.config.gravity.rotate = true;
            else
                lw.config.gravity.rotate = false;
        return true;
    }

    return false;
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
}

void ResetMinValues()
{
    // <cgerstle> equalizer min
    int minimum = 255;
    if (lw.config.equalizer.color.r > 0)
        minimum = min(minimum, lw.config.equalizer.color.r);
    if (lw.config.equalizer.color.g > 0)
        minimum = min(minimum, lw.config.equalizer.color.g);
    if (lw.config.equalizer.color.b > 0)
        minimum = min(minimum, lw.config.equalizer.color.b);

    float scale = ((float)lw.config.main.minBrightness)/((float)minimum);
    lw.config.equalizer.minColor.r = (byte)(scale * (float)lw.config.equalizer.color.r);
    lw.config.equalizer.minColor.g = (byte)(scale * (float)lw.config.equalizer.color.g);
    lw.config.equalizer.minColor.b = (byte)(scale * (float)lw.config.equalizer.color.b);

}
