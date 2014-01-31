/*****************************************************************************
 * LW.cpp
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include  "LW.h"

void LW::initLegs(WalkingModeEnum m)
{
    Serial.print("total LED count: "); Serial.println(LED_COUNT);
    _legs[0].init(&config, "left leg", ADXL_ONE, _mode, &_adxl, LEG_PIXEL_COUNT, LEG_HALF, leds);
    Serial.println("    left leg");
    _legs[1].init(&config, "right leg", ADXL_TWO, _mode, &_adxl, LEG_PIXEL_COUNT, LEG_HALF, &(leds[LEG_PIXEL_COUNT]));
    Serial.println("    right leg");
    _legs[2].init(&config, "left arm", ADXL_THREE, _mode, &_adxl, LEFT_ARM_PIXEL_COUNT, LEFT_ARM_HALF, &(leds[LEG_PIXEL_COUNT * 2]));
    Serial.println("    left arm");
    _legs[3].init(&config, "right arm", ADXL_FOUR, _mode, &_adxl, RIGHT_ARM_PIXEL_COUNT, RIGHT_ARM_HALF, &(leds[LEG_PIXEL_COUNT * 2 + LEFT_ARM_PIXEL_COUNT]));
    Serial.println("    right arm");

    _mode = m;
    setMode(_mode);
}

void LW::setMode(WalkingModeEnum m)
{
    _mode = m;
    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].setWalkingMode(_mode);

    switch (_mode)
    {
        case equalizer:
            equalizerBaseline();
            break;
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
    byte eqLevel = 0;

    // <gerstle> only listen to the EQ once per frame
    if (_mode == equalizer)
        eqLevel = equalizerListen();

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
                    pulseValue = config.pulse.syncLegsTimer;
                    if (config.pulse.syncLegsTimer >= (_pulse_length * 2))
                    {
                        _pulse_length = random(config.pulse.minPulseTime, config.pulse.maxPulseTime);
                        config.pulse.syncLegsTimer = 0;
                        pulseValue = config.pulse.syncLegsTimer;
                        pulseChangeColor = true;
                        pulseDimming = false;
                    }
                    else if (config.pulse.syncLegsTimer >= _pulse_length)
                    {
                        pulseValue = (_pulse_length * 2) - config.pulse.syncLegsTimer;
                        pulseDimming = true;
                    }
                
                    _legs[i].pulseLegMode.setSyncData(_pulse_length, pulseValue, pulseChangeColor, pulseDimming);
                }

                _legs[i].frame();
                break;

            // ------------------------------------------------------------------------
            // Sparkle
            // ------------------------------------------------------------------------
            case sparkle:
                _legs[i].frame();
                break;

            // ------------------------------------------------------------------------
            // Equalizer
            // ------------------------------------------------------------------------
            case equalizer:
                _legs[i].equalizerLegMode.eqLevel = eqLevel;
                _legs[i].frame();
                break;

            // ------------------------------------------------------------------------
            // Gravity
            // ------------------------------------------------------------------------
            case gravity:
                _legs[i].frame();
                break;

            //------------------------------------------------------------------------
            // Bubble
            //------------------------------------------------------------------------
            case bubble:
                _legs[i].frame();
                break;

            //------------------------------------------------------------------------
            // Rainbow
            //------------------------------------------------------------------------
            case rainbow:
                _legs[i].frame();
                break;
            
            //------------------------------------------------------------------------
            // Chaos
            //------------------------------------------------------------------------
            case chaos:
                break;
            
            //------------------------------------------------------------------------
            // Flames
            //------------------------------------------------------------------------
            case flames:
                break;
        }
    }
    LEDS.show();
}

void LW::equalizerBaseline()
{
    int tmp, value;
    int eqValueTotal = 0;
    int eqValueIndex = 0;
    int peak = 0;

    digitalWrite(AUDIO_RESET_PIN, HIGH);
    digitalWrite(AUDIO_RESET_PIN, LOW);
    for (eqValueIndex = 0; eqValueIndex < EQ_EMA_N; eqValueIndex++)
    {
        for (int i = 0; i < 7; i++)
        {
            digitalWrite(AUDIO_STROBE_PIN, LOW);
            delayMicroseconds(5); // to allow the output to settle
            tmp = analogRead(AUDIO_PIN);

            if (config.equalizer.allBands)
                value = max(value, tmp);
            else if (i == 0)
                value = tmp;

            digitalWrite(AUDIO_STROBE_PIN, HIGH);
        }
        eqValueTotal += value;
        peak = max(peak, value);
    }
    
    eqEMA = eqValueTotal / EQ_EMA_N;
    eqEMAPeak = peak;
}

// <gerstle> returns a byte between 0 and 100
byte LW::equalizerListen()
{
    int level = 0;
    int tmp;
    static bool emaCheck = true;

// <gerstle> might want to renable this if I'm seeing
// performance issues with the microphone
//     if (_lastEQReading < 3)
//         return;
// 
//     _lastEQReading = 0;

    // <gerstle> get the value
    digitalWrite(AUDIO_RESET_PIN, HIGH);
    digitalWrite(AUDIO_RESET_PIN, LOW);

    for (int i = 0; i < 7; i++)
    {
        digitalWrite(AUDIO_STROBE_PIN, LOW);
        delayMicroseconds(3); // to allow the output to settle

        tmp = analogRead(AUDIO_PIN);
        if (config.equalizer.allBands)
            level = max(level, tmp);
        else if (i == 0)
            level = tmp;
        digitalWrite(AUDIO_STROBE_PIN, HIGH);
    }

    if ((level < eqNminus1) && (eqNminus1 > eqNminus2) && (level > eqEMA))
    {
        // <cgerstle> artificially inflate the max...
        tmp = (1024 - level) / 4 + level; 
        eqEMAPeak = (float)(tmp - eqEMAPeak) * ((float)2 / (float)(EQ_EMA_PEAK_N + 1)) + eqEMAPeak;
    }

    if (emaCheck)
        eqEMA = (float)(level - eqEMA) * ((float)2 / (float)(EQ_EMA_N + 1)) + eqEMA;
    emaCheck = !emaCheck;

    eqNminus2 = eqNminus1;
    eqNminus1 = level;
    
    if (level < config.equalizer.rmsThreshold)
        level = 0;

    return max(0, map(level, eqEMA, eqEMAPeak, 0, 100));
//     Serial.print(level); Serial.print("\t"); Serial.print(eqEMA); Serial.print("\t"); Serial.print(eqEMAPeak); Serial.print("\t"); Serial.println(eqLevel);
}
