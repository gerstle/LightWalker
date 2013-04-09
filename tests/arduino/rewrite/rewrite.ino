#include <SoftwareSerial.h>
#include <Wire.h>
#include <ADXL345.h>
#include <SPI.h>
#include <TCL.h>
#include "colors.h"
#include "LWConfigs.h"
#include "Leg.h"
#include "LW.h"

// <gerstle> TX is the arduino's TX... attach to bluetooth RX
// <gerstle> RX is the arduino's RX... attach to bluetooth TX
#define TXPIN 10
#define RXPIN 9 

#define LEG_COUNT 2
#define PIXELS_PER_LEG 25

//RGB pixels[LEG_COUNT][PIXELS_PER_LEG];
Leg legs[LEG_COUNT] =
{
    Leg("left leg", 2, Pulse),
    Leg("right leg", 3, Pulse)
};
LW lw(1);
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

    // <gerstle> ask for initial settings for current mode
    lw.setMode(legs, MasterOff);
    bluetooth.print("SettingsPlease\r");
    Serial.println("Walking!");
}

String msg = "";
void loop()
{
    // <cgerstle> get commands from bluetooth
    if (bluetooth.available())
    {
        msg = "";
        char lastRead = bluetooth.read();
        msg.concat(lastRead);

        // <gerstle> seems like the \r gets horked sometimes and that the message
        // sent has a funny y attached to the end. So, check that we're within
        // valid ascii. If not, consider that the end of the message
        while (lastRead != '\r')
        {
            lastRead = bluetooth.read();
            if ((lastRead > (char)0x20) && (lastRead < (char)0x7F))
                msg.concat(lastRead);
        }

        if (ExecuteCommand(String(msg)))
            bluetooth.print("OK\r");
    }

    // <gerstle> perform LightWalker action
    lw.walk(legs);
}
 
bool ExecuteCommand(String input)
{
    int equalsIndex = input.indexOf('=');

    if (equalsIndex <= 0)
        return false;

    String key = input.substring(0, equalsIndex);
    String value = input.substring(equalsIndex + 1);

    Serial.print("key: "); Serial.print(key); Serial.print(" value: "); Serial.println(value);

    // ------------------------------------------------------------------------
    // MAIN
    // ------------------------------------------------------------------------
    if (key == "mainPrefMaxBrightness")
    {
        LWConfigs.main.maxBrightness = value.toInt();
        Serial.print("max set to: "); Serial.println(LWConfigs.main.maxBrightness);
    }

    // ------------------------------------------------------------------------
    // Mode
    // ------------------------------------------------------------------------
    else if (key == "mode")
    {
        WalkingModeEnum mode;

        if (value == "gravity")
            mode = Gravity;
        else if (value == "equalizer")
            mode = Equalizer;
        else if (value == "sparkle")
            mode = Sparkle;
        else if (value == "pulse")
            mode = Pulse;
        else
            return false;

        //lw.setMode(mode);
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
    else if (key == "sparklePrefFootFlashColor")
    {
        LWConfigs.sparkle.footFlashColor.r = ParseColor(value);
        LWConfigs.sparkle.footFlashColor.g = ParseColor(value);
        LWConfigs.sparkle.footFlashColor.b = ParseColor(value);
    }
    else if (key == "sparklePrefFootSparkleColor")
    {
        LWConfigs.sparkle.footSparkleColor.r = ParseColor(value);
        LWConfigs.sparkle.footSparkleColor.g = ParseColor(value);
        LWConfigs.sparkle.footSparkleColor.b = ParseColor(value);
    }
    else if (key == "sparklePrefLegSparkleColor")
    {
        LWConfigs.sparkle.legSparkleColor.r = ParseColor(value);
        LWConfigs.sparkle.legSparkleColor.g = ParseColor(value);
        LWConfigs.sparkle.legSparkleColor.b = ParseColor(value);
    }
    else if (key == "sparklePrefFlashLength")
        LWConfigs.sparkle.flashLength = value.toInt();
    else if (key == "sparklePrefSparkleLength")
        LWConfigs.sparkle.sparkleLength = value.toInt();
    else if (key == "sparklePrefFootDownFadeRate")
        LWConfigs.sparkle.footDownFadeRate = byte(value.toInt());
    else if (key == "sparklePrefFootUpFadeRate")
        LWConfigs.sparkle.footUpFadeRate = byte(value.toInt());

    // ------------------------------------------------------------------------
    // PULSE
    // ------------------------------------------------------------------------
    else if (key == "pulsePrefColor")
    {
        LWConfigs.pulse.color.r = ParseColor(value);
        LWConfigs.pulse.color.g = ParseColor(value);
        LWConfigs.pulse.color.b = ParseColor(value);
    }
    else if (key == "pulsePrefMinRate")
        LWConfigs.pulse.minPulseTime = value.toInt();
    else if (key == "pulsePrefMaxRate")
        LWConfigs.pulse.maxPulseTime = value.toInt();
    else if (key == "pulsePrefRandomColor")
        if (value == "true")
            LWConfigs.pulse.randomColor = true;
        else
            LWConfigs.pulse.randomColor = false;
    else if (key == "pulsePrefSyncLegs")
        if (value == "true")
            LWConfigs.pulse.syncLegs = true;
        else
            LWConfigs.pulse.syncLegs = false;

    return true;
}

// <gerstle> just get the first integer off of the string and return it
byte ParseColor(String colorString)
{
    int commaIndex = colorString.indexOf(',');

    colorString = colorString.substring(commaIndex + 1);
    return byte(colorString.substring(0, commaIndex).toInt());
}
