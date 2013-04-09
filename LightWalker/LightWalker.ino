// <gerstle> pogo.ino
//
// sketch for the LightWalker costume
// The main work is in the LW class

#include <SoftwareSerial.h>
#include <Wire.h>
#include <ADXL345.h>
#include <SPI.h>
#include <TCL.h>
#include "LWUtils.h"
#include "LW.h"

LW lw;
SoftwareSerial bluetooth(RXPIN, TXPIN);

void setup()
{
    // <gerstle> general setup
    Serial.begin(9600);
    Serial.println("...");
    randomSeed(long(millis()));

    // <gerstle> bluetooth setup
    pinMode(RXPIN, INPUT);
    pinMode(TXPIN, OUTPUT);
    bluetooth.begin(57600);

    // <gerstle> lights setup
    TCL.begin();
    TCL.setupDeveloperShield();
    TCL.sendEmptyFrame();

    // <gerstle> audio setup
    pinMode(AUDIO_LEFT_PIN, INPUT);
    pinMode(AUDIO_STROBE_PIN, OUTPUT);
    pinMode(AUDIO_RESET_PIN, OUTPUT);
    analogReference(DEFAULT);

    digitalWrite(AUDIO_RESET_PIN, LOW);
    digitalWrite(AUDIO_STROBE_PIN, HIGH);

    lw.initLegs(masterOff);

    if (bluetooth.isListening())
        bluetooth.print("SettingsPlease\r");

    Serial.println("Walking!");
}

const int msgLength = 128;
char msg[msgLength];
char *pKey = NULL;
char *pValue = NULL;
void loop()
{
    // <cgerstle> get commands from bluetooth
    if (bluetooth.available())
    {
        int i = 0;
        msg[i++] = (char)bluetooth.read();
        
        // <gerstle> seems like the \r gets horked sometimes and that the message
        // sent has a funny y attached to the end. So, check that we're within
        // valid ascii. If not, consider that the end of the message
        while ((msg[i - 1] != '\r') && (msg[i - 1] > 0x0) && (msg[i - 1] <= 0x7F) && (i < msgLength))
            msg[i++] = (char)bluetooth.read();
        msg[i - 1] = '\0';

        if (i > 1)
        {
            pValue = strchr(msg, '=');
            if (pValue != NULL)
            {
                pKey = msg;
                *pValue = '\0';
                pValue++;

                ExecuteCommand(atoi(pKey), pValue, i - (pValue - pKey));
//                 if (ExecuteCommand(atoi(pKey), pValue, i - (pValue - pKey) - 1))
//                     bluetooth.print("OK\r");
            }
        }

        memset(msg, '\0', i - 1);
    }

    // <gerstle> perform LightWalker action
    lw.walk();
}

bool ExecuteCommand(int key, char *value, int valueLen)
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
                lw.config.pulse.randomColor = true;
            else
                lw.config.pulse.randomColor = false;
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
