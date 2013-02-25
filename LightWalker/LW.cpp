/*****************************************************************************
 * LW.cpp
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include  "LW.h"

// ----------------------------------------------------------------------------
// LW Class
// ----------------------------------------------------------------------------

LW::LW(int x)
{
    // <gerstle> ignore the x, something about the arduino compiler
    // needing a single int class constructor. Or I don't know
    // what I'm doing.
}

void LW::initLegs(Leg* legs, WalkingModeEnum mode)
{
    _mode = mode;
    _legs = legs;
    TCL.sendEmptyFrame();
    setMode(_mode);
}

void LW::setMode(WalkingModeEnum mode)
{
    _mode = mode;
    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].setWalkingMode(mode);

    if (DEBUG)
    {
        switch (mode)
        {
            case Pulse:
                Serial.println("pulse configs");
                Serial.print("    color: "); Serial.print(LWConfigs.pulse.color.r); Serial.print(" - "); Serial.print(LWConfigs.pulse.color.g); Serial.print(" - "); Serial.println(LWConfigs.pulse.color.b);
                Serial.print("    min bright: "); Serial.println(LWConfigs.pulse.minBrightness);
                Serial.print("    max bright: "); Serial.println(LWConfigs.pulse.maxBrightness);
                Serial.print("    min pulse time: "); Serial.println(LWConfigs.pulse.minPulseTime);
                Serial.print("    max pulse time: "); Serial.println(LWConfigs.pulse.maxPulseTime);
                break;
            case Gravity:
                break;
        }
    }
}

void LW::off()
{
    TCL.sendEmptyFrame();
    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].off();
}

// <gerstle> check for inputs
void LW:: walk()
{
    TCL.sendEmptyFrame();

    unsigned long currentTime = millis();
    bool displayStatus = false;
    if (currentTime > (_laststatus + 2000))
        displayStatus = false;

    // <gerstle> foreach leg, check the sensor
    for (int i = 0; i < LEG_COUNT; i++)
    {
        bool sensorState = digitalRead(_legs[i].trigger_pin);
        
        if (displayStatus && DEBUG)
        {
            Serial.print("    leg "); Serial.println(_legs[i].name);
            Serial.print("        pin: "); Serial.println(_legs[i].trigger_pin);
            Serial.print("        pin state: "); Serial.println(sensorState);
            Serial.print("        status: "); Serial.println(_legs[i].status);
            _laststatus = millis();
        }

        switch (_mode)
        {
            case Sparkle:

                //if ((sensorState == LOW) && (_legs[i].status != Down))
                if ((sensorState == HIGH) && (_legs[i].status != Down))
                    _legs[i].sparkle_footdown();
                //else if ((sensorState == HIGH) && (_legs[i].status != Up))
                else if ((sensorState == LOW) && (_legs[i].status != Up))
                    _legs[i].sparkle_footup();
                else
                    _legs[i].sparkle_sameStatus();

                if (digitalRead(TCL_SWITCH2) == HIGH)
                    _legs[i].sparkle_fade_on = true;
                else
                    _legs[i].sparkle_fade_on = false;
                break;

            case Gravity:
                break;

            case Equalizer:
                break;

            case Pulse:
                _legs[i].pulse_pulse();    
                break;
        }
    }
    TCL.sendEmptyFrame();
}
