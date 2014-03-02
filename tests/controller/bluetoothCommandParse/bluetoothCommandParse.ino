#include <SoftwareSerial.h>
#include <Wire.h>
#include <ADXL345.h>
#include <SPI.h>
#include <TCL.h>
#include "colors.h"

#define TXPIN 11
#define RXPIN 10
 
SoftwareSerial bluetooth(RXPIN, TXPIN);
 
void setup()
{
    pinMode(RXPIN, INPUT);
    pinMode(TXPIN, OUTPUT);

    Serial.begin(9600);
    bluetooth.begin(57600);
 
    Serial.println("Walking!");
}

 
const int msgLength = 64;
char msg[msgLength];
int i = 0;
void loop()
{
    if (bluetooth.available())
    {
        i = 0;
        msg[i++] = (char)bluetooth.read();

        // <gerstle> seems like the \r gets horked sometimes and that the message
        // sent has a funny y attached to the end. So, check that we're within
        // valid ascii. If not, consider that the end of the message
        while ((msg[i - 1] != '\r') && (msg[i - 1] > 0x0) && (msg[i - 1] <= 0x7F))
            msg[i++] = (char)bluetooth.read();

        //Serial.print("received: ");
        //Serial.println(msg);

        if (ParseCommand(String(msg)))
            bluetooth.print("OK\r");
        else
            bluetooth.print("--\r");

        for (int j = 0; ((j < i) && (j < msgLength)); j++)
            msg[j] = '\0';
    }
    if (Serial.available())
        bluetooth.print((char)Serial.read());
}

enum ModeEnum
{
    Gravity,
    Equalizer,
    Sparkle
};

bool ParseCommand(String input)
{
    int colonIndex = input.indexOf(':');
    String modeString = input.substring(0, colonIndex);
    ModeEnum mode;

    //Serial.print("colonIndex: "); Serial.println(colonIndex);
    //Serial.print("mode string: "); Serial.println(modeString);

    if (modeString == "gravity")
        mode = Gravity;
    else if (modeString == "equalizer")
        mode = Equalizer;
    else if (modeString == "sparkle")
        mode = Sparkle;
    else
        return false;

    switch (mode)
    {
        case Gravity:
            Serial.println("gravity");
            break;
        case Equalizer:
            Serial.println("equalizer");
            break;
        case Sparkle:
            Serial.println("sparkle");
            break;
    }

    if (input.substring(colonIndex + 1, colonIndex + 3) == "GO")
        Serial.println("go!");
    else
    {
        int equalsIndex = input.indexOf('=');
        String key = input.substring(colonIndex + 1, equalsIndex);
        String value = input.substring(equalsIndex + 1);

        Serial.print("key: "); Serial.print(key); Serial.print(" value: "); Serial.println(value);

        switch (mode)
        {
            case Gravity:
                RGB color;
                if (key == "x")
                    ParseColor(value, &color); 
                else if (key == "y")
                    ParseColor(value, &color); 
                else if (key == "z")
                    ParseColor(value, &color); 

                Serial.print(key); Serial.print(" -> "); Serial.print(color.r); Serial.print(","); Serial.print(color.g); Serial.print(","); Serial.println(color.b);
                break;
                /*
            case Equalizer:
                break;
            case Sparkle:
                break;
                */
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
