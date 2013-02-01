// <gerstle> pogo.ino
//
// sketch for the LightWalker costume AKA pogo.
// The main work is in the LightWalker class.

#include <SPI.h>
#include <TCL.h>
#include "LWUtils.h"
#include "LightWalker.h"

LightWalker lw(25);

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
    lw.walk();
}
