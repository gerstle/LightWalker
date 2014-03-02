/*****************************************************************************
 * LegPulse.cpp
 *
 * This encapsulates the pulse leg mode
 *
 * Copyright 2014 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include "PulseLegMode.h"

void PulseLegMode::setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p)
{
    if (_setup_complete)
        return;

    // <cgerstle> should probably be doing this in a super constructor...
    _config = c;
    _legName = n;
    _channel = i2c_channel;
    _adxl = adxl;
    _pixelCount = count;
    _half = half;
    _pixels = p;

    _pulseColor = _config->pulse.color;
    _isDimming = false;
    _pulseLength = random(_config->pulse.minPulseTime, _config->pulse.maxPulseTime);

    _setup_complete = true;
}

void PulseLegMode::setSyncData(int syncLength, int syncValue, bool changeColor, bool dimming)
{
    _syncLength = syncLength;
    _syncValue = syncValue;
    _changeColor = changeColor;
    _isDimming = dimming;
}

void PulseLegMode::frame()
{
    unsigned long currentTime = millis();

    if (_config->pulse.syncLegs)
        _pulseLength = _syncLength;
    else
        _changeColor = false;

    if (!_config->pulse.syncLegs)
        //if (_lastChangeTimer >= (_pulseLength * 2))
        if (currentTime >= _lastChangeTimer + (_pulseLength * 2))
        {
            _lastChangeTimer = currentTime;
            if (!_config->pulse.syncLegs)
            {
                _pulseLength = random(_config->pulse.minPulseTime, _config->pulse.maxPulseTime);
            }

            _isDimming = false;
            _changeColor = true;
        }
        else if (currentTime >= (_lastChangeTimer + _pulseLength))
            _isDimming = true;

    if (_changeColor)
    {
        switch (_config->pulse.mode)
        {
            case staticColor:
                _pulseColor = _config->pulse.color;
                break;
            case cycleColor:
                if (_pulseColor.hue == 255)
                    _pulseColor.hue = 0;
                else
                    _pulseColor.hue += 3;
                break;
            case randomColor:
                _pulseColor.hue = random(0, 256);
                break;
        }
    }

    int value = 0;
    if (_config->pulse.syncLegs)
        value = _syncValue;
    else
        if (_isDimming)
            value = (_pulseLength * 2) - (currentTime - _lastChangeTimer);
        else
            value = currentTime - _lastChangeTimer;


    float brightness = ((float)map(value, 0, _pulseLength, 25, 100) / 100);
    brightness = brightness * brightness * brightness;
    _pulseColor.value = brightness * 256;

    for (int i = 0; i < _pixelCount; i++)
        _pixels[i] = _pulseColor;
}
