// <gerstle> 1st attempt at making a single leg
// pogo light show

#include <StandardCplusplus.h>
#include <vector>
#include <iterator>
#include <SPI.h>
#include <TCL.h>
#include "LWUtils.h"
#include "LightWalker.h"

LightWalker lw(1);

void setup()
{
    Serial.begin(9600);

    TCL.begin();
    TCL.setupDeveloperShield();
    TCL.sendEmptyFrame();

    vector<int> pins;
    pins.push_back(TCL_MOMENTARY2);

    vector<int> pixel_counts;
    pixel_counts.push_back(50);

    lw.InitLegs(pins, pixel_counts);
}

void loop()
{
    lw.walk();
}
