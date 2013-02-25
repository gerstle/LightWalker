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

Leg legs[LEG_COUNT];
LW lw(150);
SoftwareSerial bluetooth(RXPIN, TXPIN);

void setup()
{
    // <gerstle> general setup
    randomSeed(long(millis()));
    Serial.begin(9600);

    // <gerstle> bluetooth setup
    pinMode(RXPIN, INPUT);
    pinMode(TXPIN, OUTPUT);
    bluetooth.begin(57600);
 
    Serial.println("Walking!");

    // <gerstle> lights setup
    TCL.begin();
    TCL.setupDeveloperShield();
    TCL.sendEmptyFrame();

    // <gerstle> LightWalker control setup
    legs[0].Init("left leg", 2, Pulse);
    legs[1].Init("right leg", 2, Pulse);
    lw.initLegs((Leg *) &legs, Pulse);
}

const int msgLength = 64;
char msg[msgLength];
int i = 0;
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

        //Serial.print("received: ");
        //Serial.println(msg);

        if (ExecuteCommand(String(msg)))
            bluetooth.print("OK\r");

        for (int j = 0; ((j < i) && (j < msgLength)); j++)
            msg[j] = '\0';
    }

    // <gerstle> perform LightWalker action
    lw.walk();
}

bool ExecuteCommand(String input)
{
    int colonIndex = input.indexOf(':');
    String modeString = input.substring(0, colonIndex);
    WalkingModeEnum mode;

    //Serial.print("colonIndex: "); Serial.println(colonIndex);
    //Serial.print("mode string: "); Serial.println(modeString);

    if (modeString == "Gravity")
        mode = Gravity;
    else if (modeString == "Equalizer")
        mode = Equalizer;
    else if (modeString == "Sparkle")
        mode = Sparkle;
    else if (modeString == "Pulse")
        mode = Pulse;
    else if (modeString == "Main")
        mode = Main;
    else
        return false;

    Serial.println(modeString);

    if (input.substring(colonIndex + 1, colonIndex + 3) == "GO")
    {
        Serial.println("go!");
        lw.setMode(mode);
    }
    else
    {
        int equalsIndex = input.indexOf('=');
        String key = input.substring(colonIndex + 1, equalsIndex);
        String value = input.substring(equalsIndex + 1);

        Serial.print("key: "); Serial.print(key); Serial.print(" value: "); Serial.println(value);

        switch (mode)
        {
            case Main:
                if (key == "mainPrefMaxBrightness")
                    LWConfigs.main.maxBrightness = value.toInt();
                    Serial.print("max set to: "); Serial.println(LWConfigs.main.maxBrightness);
                break;
            case Gravity:
//                 if (key == "x")
//                     ParseColor(value, &color); 
//                 else if (key == "y")
//                     ParseColor(value, &color); 
//                 else if (key == "z")
//                     ParseColor(value, &color); 

                //Serial.print(key); Serial.print(" -> "); Serial.print(color.r); Serial.print(","); Serial.print(color.g); Serial.print(","); Serial.println(color.b);
                break;
            case Equalizer:
                break;
            case Sparkle:
                if (key == "sparklePrefFootFlashColor")
                    ParseColor(value, &(LWConfigs.sparkle.footFlashColor)); 
                else if (key == "sparklePrefFootSparkleColor")
                    ParseColor(value, &(LWConfigs.sparkle.footSparkleColor)); 
                else if (key == "sparklePrefLegSparkleColor")
                    ParseColor(value, &(LWConfigs.sparkle.legSparkleColor)); 
                else if (key == "sparklePrefFlashLength")
                    LWConfigs.sparkle.flashLength = value.toInt();
                else if (key == "sparklePrefSparkleLength")
                    LWConfigs.sparkle.sparkleLength = value.toInt();
                else if (key == "sparklePrefFootDownFadeRate")
                    LWConfigs.sparkle.footDownFadeRate = byte(value.toInt());
                else if (key == "sparklePrefFootUpFadeRate")
                    LWConfigs.sparkle.footUpFadeRate = byte(value.toInt());
                break;
            case Pulse:
                if (key == "pulsePrefColor")
                    ParseColor(value, &(LWConfigs.pulse.color)); 
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
                break;
        }

    }

    return true;
}

void ParseColor(String colorString, RGB *color)
{
    int commaIndex = colorString.indexOf(',');

    (*color).r = byte(colorString.substring(0, commaIndex).toInt());
    
    colorString = colorString.substring(commaIndex + 1);
    commaIndex = colorString.indexOf(',');

    (*color).g = byte(colorString.substring(0, commaIndex).toInt());
    (*color).b = byte(colorString.substring(commaIndex + 1).toInt());
}
