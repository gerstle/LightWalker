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

    randomSeed(long(millis()));

    vector<int> pins;
    pins.push_back(TCL_MOMENTARY1);
    pins.push_back(TCL_MOMENTARY2);

    vector<int> pixel_counts;
//     pixel_counts.push_back(50);
    pixel_counts.push_back(25);
    pixel_counts.push_back(25);

    Serial.println("CLG1");
    Serial.println(pins.size());
    Serial.println(pixel_counts.size());
    lw.initLegs(pins, pixel_counts);
    Serial.println("CLG2");
}

void loop()
{
//     Serial.println("CLG3");
    lw.walk();
//     Serial.println("CLG4");
//     delay(500);
}
