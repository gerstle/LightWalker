// <gerstle> pogo.ino
//
// sketch for the LightWalker costume
// The main work is in the LW class

#include <ADXL345.h>
#include <Wire.h>
#include <SPI.h>
#include <TCL.h>
#include "LWUtils.h"
#include "LW.h"

LW lw;

// <gerstle> bluetooth comms
const int msgLength = 64;
char msg[msgLength];
int msgIndex = 0;
char *pKey = NULL;
char *pValue = NULL;

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

    // <cgerstle> ADXL setup
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
            pValue = strchr(msg, '=');
            if (pValue != NULL)
            {
                pKey = msg;
                *pValue = '\0';
                pValue++;

                if (executeCommand(atoi(pKey), pValue, msgIndex - (pValue - pKey) - 1))
                    Serial1.print("OK\r");
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
        case prefMainMaxBrightness:
            lw.config.main.maxBrightness = atoi(value);
            break;

        // ------------------------------------------------------------------------
        // Mode 
        // ------------------------------------------------------------------------
        case prefMode:
            valueInt = atoi(value);
            WalkingModeEnum newMode;

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
            }

            lw.setMode(newMode);
            break;

        // ------------------------------------------------------------------------
        // Sparkle
        // ------------------------------------------------------------------------
        case prefSparkleFootUpFadeRate:
            lw.config.sparkle.footUpFadeRate = atoi(value);
            break;  
        case prefSparkleFootDownFadeRate:
            lw.config.sparkle.footDownFadeRate = atoi(value);
            break;
        case prefSparkleFlashLength:
            lw.config.sparkle.flashLength = 200; //atoi(value);
            break;
        case prefSparkleSparkleLength:
            lw.config.sparkle.sparkleLength = atoi(value);
            break;
        case prefSparkleFootFlashColor:
            ParseColor(value, &(lw.config.sparkle.footFlashColor));
            break;
        case prefSparkleFootSparkleColor:
            ParseColor(value, &(lw.config.sparkle.footSparkleColor));
            break;
        case prefSparkleLegSparkleColor:
            ParseColor(value, &(lw.config.sparkle.legSparkleColor));
            break;

        // ------------------------------------------------------------------------
        // PULSE
        // ------------------------------------------------------------------------
        case prefPulseMinRate:
            lw.config.pulse.minPulseTime = atoi(value);
            break;
        case prefPulseMaxRate:
            lw.config.pulse.maxPulseTime = atoi(value);
            break;
        case prefPulseRandomColor:
            if (strncmp(value, "1", valueLen) == 0)
            {
                Serial.print("random = true");
                lw.config.pulse.randomColor = true;
            }
            else
            {
                Serial.print("random = false");
                lw.config.pulse.randomColor = false;
            }
            break;

        case prefPulseSyncLegs:
            if (strncmp(value, "1", valueLen) == 0)
                lw.config.pulse.syncLegs = true;
            else
                lw.config.pulse.syncLegs  = false;
            break;
        case prefPulseColor:
        Serial.print("color before:: "); Serial.print(lw.config.pulse.color.r); Serial.print(" - "); Serial.print(lw.config.pulse.color.g); Serial.print(" - "); Serial.println(lw.config.pulse.color.b);
            ParseColor(value, &(lw.config.pulse.color));
        Serial.print("color after:: "); Serial.print(lw.config.pulse.color.r); Serial.print(" - "); Serial.print(lw.config.pulse.color.g); Serial.print(" - "); Serial.println(lw.config.pulse.color.b);
            break;

        // ------------------------------------------------------------------------
        // EQUALIZER
        // ------------------------------------------------------------------------
        case prefEqualizerColor:
            ParseColor(value, &(lw.config.equalizer.color));
            break;
//         case prefEqualizerLevel:
//             lw.config.equalizer.brightnessPercent = atoi(value);
//             lw.equalizer_listen();
//             break;
        case prefEqualizerRMSThreshold:
            lw.config.equalizer.RMSThreshold = atoi(value);
            break;
        case prefEqualizerAllLights:
            if (strncmp(value, "1", 1) == 0)
                lw.config.equalizer.allLights = true;
            else
                lw.config.equalizer.allLights = false;
            break;

        // ------------------------------------------------------------------------
        // Gravity
        // ------------------------------------------------------------------------
        case prefGravityXColor:
            ParseColor(value, &(lw.config.gravity.xColor));
            break;
        case prefGravityYColor:
            ParseColor(value, &(lw.config.gravity.yColor));
            break;
        case prefGravityZColor:
            ParseColor(value, &(lw.config.gravity.zColor));
            break;
        case prefGravityRotate:
            if (strncmp(value, "1", 1) == 0)
                lw.config.gravity.rotate = true;
            else
                lw.config.gravity.rotate = false;
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
}
