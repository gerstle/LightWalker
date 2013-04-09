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
}

void LW::setMode(Leg legs[], WalkingModeEnum mode)
{
    _mode = mode;
    _lightModeChangeTime = millis();
    for (int i = 0; i < LEG_COUNT; i++)
        legs[i].setWalkingMode(mode);

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
                Serial.print("    random color: "); Serial.println(LWConfigs.pulse.randomColor);
                Serial.print("    sync: "); Serial.println(LWConfigs.pulse.syncLegs);
                break;
            case Gravity:
                break;
        }
    }
}

// <gerstle> check for inputs
void LW:: walk(Leg legs[])
{
    TCL.sendEmptyFrame();

    if (_mode == Off)
        return;

    unsigned long currentTime = millis();
    bool displayStatus = false;
    bool pulseChangeColor = false;
    if (currentTime > (_laststatus + 2000))
        displayStatus = false;

    // <gerstle> foreach leg, check the sensor
    for (int i = 0; i < LEG_COUNT; i++)
    {
        bool sensorState = digitalRead(legs[i].trigger_pin);
        
        if (displayStatus && DEBUG)
        {
            Serial.print("    leg "); Serial.println(legs[i].name);
            Serial.print("        pin: "); Serial.println(legs[i].trigger_pin);
            Serial.print("        pin state: "); Serial.println(sensorState);
            Serial.print("        status: "); Serial.println(legs[i].status);
            _laststatus = millis();
        }

        switch (_mode)
        {
            case Sparkle:

                //if ((sensorState == LOW) && (_legs[i].status != Down))
                if ((sensorState == HIGH) && (legs[i].status != Down))
                    legs[i].sparkle_footdown();
                //else if ((sensorState == HIGH) && (_legs[i].status != Up))
                else if ((sensorState == LOW) && (legs[i].status != Up))
                    legs[i].sparkle_footup();
                else
                    legs[i].sparkle_sameStatus();

                //if (LWConfigs.
                    legs[i].sparkle_fade_on = true;
                //else
                    //_legs[i].sparkle_fade_on = false;
                break;

            case Gravity:
                break;

            case Equalizer:
                break;

            case Pulse:
                if (LWConfigs.pulse.syncLegs && (currentTime >= (_lightModeChangeTime + (_pulse_length * 2))))
                {
                    _pulse_length = random(LWConfigs.pulse.minPulseTime, LWConfigs.pulse.maxPulseTime);
                    _lightModeChangeTime = currentTime;
                    _pulse_isDimming = false;
                    pulseChangeColor = true;
                }
                else if (currentTime >= (_lightModeChangeTime + _pulse_length))
                    _pulse_isDimming = true;
                
                int value;
                if (LWConfigs.pulse.syncLegs)
                    if (_pulse_isDimming)
                        value = (_pulse_length * 2) - (currentTime - _lightModeChangeTime);
                    else
                        value = currentTime - _lightModeChangeTime;

                legs[i].pulse_pulse(currentTime, _lightModeChangeTime, _pulse_length, value, pulseChangeColor);
                break;
        }
    }
}
