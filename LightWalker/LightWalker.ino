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

// <gerstle> TX is the arduino's TX... attach to bluetooth RX
// <gerstle> RX is the arduino's RX... attach to bluetooth TX
#define TXPIN 10
#define RXPIN 9 

//RGB pixels[LEG_COUNT][PIXELS_PER_LEG];
LW lw(150);
SoftwareSerial bluetooth(RXPIN, TXPIN);

void setup()
{
    // <gerstle> general setup
    Serial.begin(9600);
    Serial.println("setting up...");
    randomSeed(long(millis()));

    // <gerstle> bluetooth setup
    pinMode(RXPIN, INPUT);
    pinMode(TXPIN, OUTPUT);
    bluetooth.begin(57600);
 

    // <gerstle> lights setup
    TCL.begin();
    TCL.setupDeveloperShield();
    TCL.sendEmptyFrame();

    lw.initLegs(Pulse);
    lw.setMode(MasterOff);
    bluetooth.print("SettingsPlease\r");
    Serial.println("Walking!");
}

const int msgLength = 128;
char msg[msgLength];
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

//         Serial.print("received: ");
//         Serial.println(msg);

        msg[i - 1] = '\0';
        if (ExecuteCommand(msg))
            bluetooth.print("OK\r");

        for (int j = 0; ((j < i) && (j < msgLength)); j++)
            msg[j] = '\0';
    }

    // <gerstle> perform LightWalker action
    lw.walk();
}

bool ExecuteCommand(char input[])
{
    char *pKey;
    char *pValue;
    pValue = strchr(input, '=');

    if (pValue == NULL)
        return false;

    pKey = input;
    *pValue = '\0';
    pValue++;

    Serial.print("key: "); Serial.print(pKey); Serial.print(" value: "); Serial.println(pValue);

    // ------------------------------------------------------------------------
    // MAIN
    // ------------------------------------------------------------------------
    if (strcmp(pKey, "mainPrefMaxBrightness") == 0)
    {
        LWConfigs.main.maxBrightness = atoi(pValue);
        Serial.print("max set to: "); Serial.println(LWConfigs.main.maxBrightness);
    }

    // ------------------------------------------------------------------------
    // Mode
    // ------------------------------------------------------------------------
    else if (strcmp(pKey, "mode") == 0)
    {
        WalkingModeEnum mode;

        if (strcmp(pValue, "gravity") == 0)
            mode = Gravity;
        else if (strcmp(pValue, "equalizer") == 0)
            mode = Equalizer;
        else if (strcmp(pValue, "sparkle") == 0)
            mode = Sparkle;
        else if (strcmp(pValue, "pulse") == 0)
            mode = Pulse;
        else
            return false;

        lw.setMode(mode);
    }

    // ------------------------------------------------------------------------
    // Gravity
    // ------------------------------------------------------------------------
//                 if (key == "x")
//                     ParseColor(value, &color); 
//                 else if (key == "y")
//                     ParseColor(value, &color); 
//                 else if (key == "z")
//                     ParseColor(value, &color); 

    // ------------------------------------------------------------------------
    // SPARKLE
    // ------------------------------------------------------------------------
    else if (strcmp(pKey, "sparklePrefFootFlashColor") == 0)
    {
//         sparkle.footFlashColor.r = ParseColor(pValue);
//         sparkle.footFlashColor.g = ParseColor(strchr(pValue, ':') + 1);
//         sparkle.footFlashColor.b = ParseColor(value);
        ParseColor(pValue, &(LWConfigs.sparkle.footFlashColor));
    }
    else if (strcmp(pKey, "sparklePrefFootSparkleColor") == 0)
    {
//         sparkle.footSparkleColor.r = ParseColor(value);
//         sparkle.footSparkleColor.g = ParseColor(value);
//         sparkle.footSparkleColor.b = ParseColor(value);
        ParseColor(pValue, &(LWConfigs.sparkle.footSparkleColor));
    }
    else if (strcmp(pKey, "sparklePrefLegSparkleColor") == 0)
    {
//         sparkle.legSparkleColor.r = ParseColor(value);
//         sparkle.legSparkleColor.g = ParseColor(value);
//         sparkle.legSparkleColor.b = ParseColor(value);
        ParseColor(pValue, &(LWConfigs.sparkle.legSparkleColor));
    }
    else if (strcmp(pKey, "sparklePrefFlashLength") == 0)
        LWConfigs.sparkle.flashLength = atoi(pValue);
    else if (strcmp(pKey, "sparklePrefSparkleLength") == 0)
        LWConfigs.sparkle.sparkleLength = atoi(pValue);
    else if (strcmp(pKey, "sparklePrefFootDownFadeRate") == 0)
        LWConfigs.sparkle.footDownFadeRate = byte(atoi(pValue));
    else if (strcmp(pKey, "sparklePrefFootUpFadeRate") == 0)
        LWConfigs.sparkle.footUpFadeRate = byte(atoi(pValue));

    // ------------------------------------------------------------------------
    // PULSE
    // ------------------------------------------------------------------------
    else if (strcmp(pKey, "pulsePrefColor") == 0)
    {
//         pulse.color.r = ParseColor(value);
//         pulse.color.g = ParseColor(value);
//         pulse.color.b = ParseColor(value);
        ParseColor(pValue, &(LWConfigs.pulse.color));
    }
    else if (strcmp(pKey, "pulsePrefMinRate") == 0)
        LWConfigs.pulse.minPulseTime = atoi(pValue);
    else if (strcmp(pKey, "pulsePrefMaxRate") == 0)
        LWConfigs.pulse.maxPulseTime = atoi(pValue);
    else if (strcmp(pKey, "pulsePrefRandomColor") == 0)
        if (strcmp(pValue, "true") == 0)
            LWConfigs.pulse.randomColor = true;
        else
            LWConfigs.pulse.randomColor = false;
    else if (strcmp(pKey, "pulsePrefSyncLegs") == 0)
        if (strcmp(pValue, "true") == 0)
            LWConfigs.pulse.syncLegs = true;
        else
            LWConfigs.pulse.syncLegs = false;

    // ------------------------------------------------------------------------
    // EQUALIZER
    // ------------------------------------------------------------------------
    else if (strcmp(pKey, "equalizerPrefColor") == 0)
        ParseColor(value, &(LWConfigs.equalizer.color)); 
    else if (strcmp(pKey, "equalizerPrefBrightness" == 0)
        LWConfigs.equalizer.brightnessPercent = value.toInt();

    return true;
}

void ParseColor(char colorString[], RGB *color)
{
    char *pValue = colorString;
    char *pComma = strchr(colorString, ',');
    *pComma = '\0';

    (*color).r = byte(atoi(pValue));

    pValue = pComma + 1;
    pComma = strchr(pValue, ',');
    *pComma = '\0';

    (*color).g = byte(atoi(pValue));
    (*color).b = byte(atoi(pComma + 1));
}

// <gerstle> just get the first integer off of the string and return it
// byte ParseColor(char colorString[])
// {
//     char *pComma = strchr(colorString, ',');
//     *pComma = '\0';
//     byte rv;
// 
//     rv = byte(atoi(colorString));
//     *pComma = ':';
// 
//     return rv;
// }
