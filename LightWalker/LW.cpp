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
    _legs[0].init(&config, "left leg", ADXL_ONE, mode, &_adxl, FULL_LEG_PIXEL_COUNT, FULL_LEG_HALF, _leftLegPixels);
    _legs[1].init(&config, "right leg", ADXL_TWO, mode, &_adxl, FULL_LEG_PIXEL_COUNT, FULL_LEG_HALF, _rightLegPixels);
    _legs[2].init(&config, "left arm", ADXL_THREE, mode, &_adxl, ARM_PIXEL_COUNT, ARM_HALF, _leftArmPixels);
    _legs[3].init(&config, "right arm", ADXL_FOUR, mode, &_adxl, ARM_PIXEL_COUNT, ARM_HALF, _rightArmPixels);

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
            case bubble:
                if (_legs[i].detectStep(&_adxl))
                    _legs[i].bubble_step();
                else
                    _legs[i].bubble_bubble();
                break;

            case sparkle:
                if (_legs[i].detectStep(&_adxl))
                    _legs[i].sparkle_footdown();
                else
                    _legs[i].sparkle_sameStatus();
                break;

            case gravity:
                _legs[i].gravity2Lights(&_adxl);
                break;

            case equalizer:
                equalizer_listen(currentTime);
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
    _lastEQReading = millis();
}

void LW::equalizer_listen(unsigned long currentTime)
{
    int level = 0;
    int tmp;
    byte r, g, b = 0;
    float brightness;
    static bool emaCheck = true;

    if (currentTime <= (_lastEQReading + 3))
        return;
    _lastEQReading = currentTime;

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
    
    if (level < config.equalizer.RMSThreshold)
        level = 0;

    brightness = (float)map(level, eqEMA, eqEMAPeak, 0, 100) / (float)100;
    if (brightness < 0.0)
        brightness = 0;
    //Serial.print(level); Serial.print("\t"); Serial.print(eqEMA); Serial.print("\t"); Serial.print(eqEMAPeak); Serial.print("\t"); Serial.println(brightness);

    if (config.equalizer.allLights)
    {
        r = byte((float)config.equalizer.maxColor.r * brightness);
        g = byte((float)config.equalizer.maxColor.g * brightness);
        b = byte((float)config.equalizer.maxColor.b * brightness);

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

void LW::setLegsOff()
{
    _legs[0].setPixelCount(NO_STILTS_LEG_PIXEL_COUNT, NO_STILTS_LEG_HALF);
    _legs[1].setPixelCount(NO_STILTS_LEG_PIXEL_COUNT, NO_STILTS_LEG_HALF);
}

void LW::setLegsOn()
{
    _legs[0].setPixelCount(FULL_LEG_PIXEL_COUNT, FULL_LEG_HALF);
    _legs[1].setPixelCount(FULL_LEG_PIXEL_COUNT, FULL_LEG_HALF);
}

void LW::setArmsOff()
{
    _legs[2].setPixelCount(0, 0);
    _legs[3].setPixelCount(0, 0);
}

void LW::setArmsOn()
{
    _legs[2].setPixelCount(ARM_PIXEL_COUNT, ARM_HALF);
    _legs[3].setPixelCount(ARM_PIXEL_COUNT, ARM_HALF);
}
