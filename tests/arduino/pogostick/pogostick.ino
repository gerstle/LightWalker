// <gerstle> 1st attempt at making a single leg
// pogo light show

#include <SPI.h>
#include <TCL.h>
#include "LWUtils.h"
#include "LightWalker.h"

LightWalker lw(1); // <gerstle> 1 is arbitrary, I can't seem to create an instance w/out this type of constructor

void setup()
{
    Serial.begin(9600);

    TCL.begin();
    TCL.setupDeveloperShield();
    TCL.sendEmptyFrame();

    randomSeed(long(millis()));

    lw.initLegs();
}

void loop()
{
//     Serial.println("CLG3");
    lw.walk();
//     Serial.println("CLG4");
//     delay(500);
}
