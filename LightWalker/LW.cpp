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

void LW::initLegs(WalkingModeEnum m)
{
    _legs[0].Init(&config, "left leg", ADXL_ONE, mode, &_adxl, LEFT_LEG_PIXEL_COUNT, 2, _leftLegPixels);
    _legs[1].Init(&config, "right leg", ADXL_TWO, mode, &_adxl, RIGHT_LEG_PIXEL_COUNT, 10, _rightLegPixels);
    _legs[2].Init(&config, "left arm", ADXL_THREE, mode, &_adxl, LEFT_ARM_PIXEL_COUNT, 2, _leftArmPixels);
    _legs[3].Init(&config, "right arm", ADXL_FOUR, mode, &_adxl, RIGHT_ARM_PIXEL_COUNT, 10, _rightArmPixels);

//     for (int i = 0; i < LEG_COUNT; i++)
//     {
//         LWUtils.selectI2CChannels(_legs[i].channel);
//         _adxl.writeTo(ADXL345_POWER_CTL, 0);      
//         _adxl.writeTo(ADXL345_POWER_CTL, 16);
//         _adxl.writeTo(ADXL345_POWER_CTL, 8); 
//         _adxl.setActivityThreshold(75); //62.5mg per increment
//         _adxl.setInactivityThreshold(75); //62.5mg per increment
//         _adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
// 
//         //look of activity movement on this axes - 1 == on; 0 == off 
//         _adxl.setActivityX(1);
//         _adxl.setActivityY(1);
//         _adxl.setActivityZ(1);
// 
//         //set values for what is considered freefall (0-255)
//         _adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
//         _adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
//     }

    mode = m;
    setMode(mode);
}

void LW::setMode(WalkingModeEnum m)
{
    mode = m;
    _lightModeChangeTime = millis();
    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].setWalkingMode(mode, &_adxl);

    switch (mode)
    {
        case equalizer:
            equalizer_baseline();
            break;
    }
}

void LW::off()
{
    TCL.sendEmptyFrame();
    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].off();
}

// <gerstle> check for inputs
void LW::walk()
{
    TCL.sendEmptyFrame();

    unsigned long currentTime = millis();
    bool displayStatus = false;
    bool pulseChangeColor = false;
    if (currentTime > (_laststatus + 2000))
        displayStatus = false;

    // <gerstle> foreach leg, check the sensor
    for (int i = 0; i < LEG_COUNT; i++)
    {
        if (displayStatus && DEBUG)
        {
//             Serial.print("    leg "); Serial.println(_legs[i].name);
//             Serial.print("        pin: "); Serial.println(_legs[i].trigger_pin);
//             Serial.print("        pin state: "); Serial.println(sensorState);
//             Serial.print("        status: "); Serial.println(_legs[i].status);
            _laststatus = millis();
        }

        // <gerstle> init leg for this run
        _legs[i].currentTime = currentTime;
        switch (mode)
        {
            case sparkle:
                _legs[i].detectStep(&_adxl);
                break;

            case gravity:
                _legs[i].gravity2Lights(&_adxl);
                break;

            case equalizer:
                equalizer_listen();
                break;

            case pulse:
                if (config.pulse.syncLegs && (currentTime >= (_lightModeChangeTime + (_pulse_length * 2))))
                {
                    _pulse_length = random(config.pulse.minPulseTime, config.pulse.maxPulseTime);
                    _lightModeChangeTime = currentTime;
                    _pulse_isDimming = false;
                    pulseChangeColor = true;
                }
                else if (currentTime >= (_lightModeChangeTime + _pulse_length))
                    _pulse_isDimming = true;
                
                int value;
                if (config.pulse.syncLegs)
                    if (_pulse_isDimming)
                        value = (_pulse_length * 2) - (currentTime - _lightModeChangeTime);
                    else
                        value = currentTime - _lightModeChangeTime;

                _legs[i].pulse_pulse(_lightModeChangeTime, _pulse_length, value, pulseChangeColor);
                break;
        }
    }
    TCL.sendEmptyFrame();
}

void LW::equalizer_baseline()
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
            delayMicroseconds(30); // to allow the output to settle
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

void LW::equalizer_listen()
{
    int level = 0;
    int tmp;
    byte r, g, b = 0;
    float brightness;

    // <gerstle> get the value
    digitalWrite(AUDIO_RESET_PIN, HIGH);
    digitalWrite(AUDIO_RESET_PIN, LOW);

    for (int i = 0; i < 7; i++)
    {
        digitalWrite(AUDIO_STROBE_PIN, LOW);
        delayMicroseconds(20); // to allow the output to settle

        tmp = analogRead(AUDIO_PIN);
        if (config.equalizer.allBands)
            level = max(level, tmp);
        else if (i == 0)
            level = tmp;
        digitalWrite(AUDIO_STROBE_PIN, HIGH);
    }

    if ((level < eqNminus1) && (eqNminus1 > eqNminus2) && (level > eqEMA))
        eqEMAPeak = (float)(level - eqEMAPeak) * ((float)2 / (float)(EQ_EMA_PEAK_N + 1)) + eqEMAPeak;

    if (random(0,1) == 0)
        eqEMA = (float)(level - eqEMA) * ((float)2 / (float)(EQ_EMA_N + 1)) + eqEMA;
//     Serial.print(level);
//     Serial.print("\t"); Serial.print(eqEMA);
//     Serial.print("\t"); Serial.println(eqEMAPeak);

    eqNminus2 = eqNminus1;
    eqNminus1 = level;
    
    if (level < config.equalizer.RMSThreshold)
        level = 0;

    brightness = map(level, eqEMA, eqEMAPeak, 0, 100) / 100;
    if (brightness < 0.0)
        brightness = 0;

    if (config.equalizer.allLights)
    {
        r = byte((float)config.equalizer.color.r * brightness);
        g = byte((float)config.equalizer.color.g * brightness);
        b = byte((float)config.equalizer.color.b * brightness);

        if ((r == 0) && (g == 0) && (b == 0))
        {
            r = config.equalizer.minColor.r;
            g = config.equalizer.minColor.g;
            b = config.equalizer.minColor.b;
        }
    }

    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].equalizer_listen(brightness, r, g, b);
}
