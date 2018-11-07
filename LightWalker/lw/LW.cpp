/*****************************************************************************
 * LW.cpp
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include  "LW.h"
#include <OctoWS2811.h>

DMAMEM int displayMemory[LEDS_PER_STRIP * 6];
int drawingMemory[LEDS_PER_STRIP * 6];
OctoWS2811 octoLeds(LEDS_PER_STRIP, displayMemory, drawingMemory, WS2811_RGB | WS2811_800kHz);

void LW::initLegs(WalkingModeEnum m)
{
	octoLeds.begin();

    Serial.print("total LED count: "); Serial.println(LED_COUNT);
    _legs[0].init(&config, "left leg", ADXL_LEFT_LEG, _mode, &_adxl, LEG_PIXEL_COUNT, LEG_HALF, leds_left_leg, 100);
    Serial.println("    left leg");
    testLeg(0, CRGB::Blue);

    _legs[1].init(&config, "right leg", ADXL_RIGHT_LEG, _mode, &_adxl, LEG_PIXEL_COUNT, LEG_HALF, leds_right_leg, 300);
    Serial.println("    right leg");
    testLeg(1, CRGB::Yellow);

    _legs[2].init(&config, "left arm", ADXL_LEFT_ARM, _mode, &_adxl, ARM_PIXEL_COUNT, ARM_HALF, leds_left_arm, 0);
    Serial.println("    left arm");
    testLeg(2, CRGB::Purple);

    _legs[3].init(&config, "right arm", ADXL_RIGHT_ARM, _mode, &_adxl, ARM_PIXEL_COUNT, ARM_HALF, leds_right_arm, 200);
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
    writeLeg(&_legs[legIndex]);
    octoLeds.show();
    delay(2000);
}

void LW::initAudio()
{
	eq.init();
}

void LW::setMode(WalkingModeEnum m)
{
    _mode = m;
    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].setWalkingMode(_mode);
}

void LW::off()
{
    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].off();
}

void LW::writeLeg(Leg *leg)
{
	for (int j = 0; j < leg->pixelCount; j++)
		octoLeds.setPixel(leg->baseIndex + j, leg->pixels[j].r, leg->pixels[j].g, leg->pixels[j].b);
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
        eqLevel = eq.listen();

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
                    {
                        if (pulseDimming)
                            pulseValue = (_pulse_length * 2) - (currentTime - config.pulse.syncLegsTimer);
                        else
                            pulseValue = currentTime - config.pulse.syncLegsTimer;
                    }
                
                    _legs[i].pulseLegMode.setSyncData(_pulse_length, pulseValue, pulseChangeColor, pulseDimming);
                }

                break;

            // ------------------------------------------------------------------------
            // Equalizer
            // ------------------------------------------------------------------------
            case equalizer:
                _legs[i].equalizerLegMode.eqLevel = eqLevel;
                break;
            default:
            	// nothing
            	break;
        }

        // <cgerstle> paint the lights.
        _legs[i].frame();
        writeLeg(&_legs[i]);
    }

    // <cgerstle> set the head to the last set of pixel values it can find that are populated
    // TODO
//    int index = LED_COUNT - HEAD_PIXEL_COUNT - 1;
//    while ((index > 0) && (leds[index].r == 0 && leds[index].g == 0 && leds[index].b == 0))
//        index--;
//
//    leds[LED_COUNT - HEAD_PIXEL_COUNT] = leds[index];
//    leds[LED_COUNT - HEAD_PIXEL_COUNT].maximizeBrightness();
//
//    for (int i = (HEAD_PIXEL_COUNT - 1); i > 0; i--)
//		leds[LED_COUNT - i] = leds[LED_COUNT - HEAD_PIXEL_COUNT];

    // <cgerstle> sends the colors out to the lights
    octoLeds.show();
}

