/*****************************************************************************
 * LW.cpp
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include  "LW.h"
#include "audio.h"

void LW::initLegs(WalkingModeEnum m)
{
    Serial.print("total LED count: "); Serial.println(LED_COUNT);
    _legs[0].init(&config, "left leg", ADXL_ONE, _mode, &_adxl, LEG_PIXEL_COUNT, LEG_HALF, leds);
    Serial.println("    left leg");
    testLeg(0, CRGB::Blue);

    _legs[1].init(&config, "right leg", ADXL_TWO, _mode, &_adxl, LEG_PIXEL_COUNT, LEG_HALF, &(leds[LEG_PIXEL_COUNT]));
    Serial.println("    right leg");
    testLeg(1, CRGB::Yellow);

    _legs[2].init(&config, "left arm", ADXL_THREE, _mode, &_adxl, LEFT_ARM_PIXEL_COUNT, LEFT_ARM_HALF, &(leds[LEG_PIXEL_COUNT * 2]));
    Serial.println("    left arm");
    testLeg(2, CRGB::Purple);

    _legs[3].init(&config, "right arm", ADXL_FOUR, _mode, &_adxl, RIGHT_ARM_PIXEL_COUNT, RIGHT_ARM_HALF, &(leds[LEG_PIXEL_COUNT * 2 + LEFT_ARM_PIXEL_COUNT]));
    Serial.println("    right arm");
    testLeg(3, CRGB::Green);

    delay(250);
    off();
    LEDS.show();

    _mode = m;
    setMode(_mode);
}

void LW::testLeg(byte legIndex, CRGB color)
{
    for (int i = 0; i < _legs[legIndex].pixelCount; i++)
        _legs[legIndex].pixels[i] = color;
    delay(350);
    LEDS.show();
}

void LW::setMode(WalkingModeEnum m)
{
    _mode = m;
    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].setWalkingMode(_mode);

    switch (_mode)
    {
        case equalizer:
            equalizerBaseline(&config);
    }
}

void LW::off()
{
    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].off();
}

// <gerstle> check for inputs
void LW::walk()
{
    bool pulseChangeColor = false;
    bool pulseDimming = false;
    int pulseValue = 0;
    float eqLevel = 0.0;
    unsigned long currentTime = millis();

    // <gerstle> only listen to the EQ once per frame
    if (_mode == equalizer)
        eqLevel = equalizerListen(&config);

    // <gerstle> foreach leg, check the sensor
    for (int i = 0; i < LEG_COUNT; i++)
    {
        // <gerstle> init leg for this run
        switch (_mode)
        {
            // ------------------------------------------------------------------------
            // Pulse
            // ------------------------------------------------------------------------
            case pulse:

                if (config.pulse.syncLegs)
                {
                    if (currentTime >= (config.pulse.syncLegsTimer + (_pulse_length * 2)))
                    {
                        _pulse_length = random(config.pulse.minPulseTime, config.pulse.maxPulseTime);
                        config.pulse.syncLegsTimer = currentTime;
                        pulseDimming = false;
                        pulseChangeColor = true;
                    }
                    else if (currentTime >= (config.pulse.syncLegsTimer + _pulse_length))
                        pulseDimming = true;
                    
                    if (config.pulse.syncLegs)
                        if (pulseDimming)
                            pulseValue = (_pulse_length * 2) - (currentTime - config.pulse.syncLegsTimer);
                        else
                            pulseValue = currentTime - config.pulse.syncLegsTimer;
                
                    _legs[i].pulseLegMode.setSyncData(_pulse_length, pulseValue, pulseChangeColor, pulseDimming);
                }

                break;

            // ------------------------------------------------------------------------
            // Equalizer
            // ------------------------------------------------------------------------
            case equalizer:
                _legs[i].equalizerLegMode.eqLevel = eqLevel;
                break;
        }

        // <cgerstle> paint the lights.
        _legs[i].frame();
    }

    // <cgerstle> sends the colors out to the lights
    LEDS.show();
}

