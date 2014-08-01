/*****************************************************************************
 * LegEqualizer.cpp
 *
 * This encapsulates the equalizer leg mode
 *
 * Copyright 2014 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include "EqualizerLegMode.h"
#include "LWUtils.h"

void EqualizerLegMode::setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p)
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

    _perlinZ = (double) random8() / (double) 25500;

    _setup_complete = true;
}

// <cgerstle> eqLevel is 0.0 -> 1.0
void EqualizerLegMode::frame()
{
    int i = 0;
    int lower_threshold = _half - (eqLevel * _half);
    int upper_threshold = (eqLevel * _half) + _half;
    double half_min = (double)_config->equalizer.minValue / 2;
    unsigned long currentTime = millis();
    bool recalculateNoise = false;

    if (currentTime > (_lastChangeTimer + 50))
    {
        _lastChangeTimer = currentTime;
        _perlinZ += 0.001;
        recalculateNoise = true;
    }

    for (i = 0; i < _pixelCount; i++)
    {
        // <cgerstle> this is the "OFF"
        if (((i < _half) && (i < lower_threshold)) ||
            ((i > _half) && (i > upper_threshold)))
        {
            if (_config->equalizer.minValue > 0)
            {
                if (recalculateNoise)
                {
                    _perlinZ += 0.0005;
                    double noiseModifier = LWUtils.perlinNoise(_perlinZ + sin(i /  4), cos(_perlinZ), _perlinZ);
                    _previousValues[i] = (noiseModifier * half_min) + half_min;
                }

                if (_config->equalizer.mode == EQSingleRainbow)
                    _pixels[i].setHSV(map(i, 0, _pixelCount, 0, 255), 255, _previousValues[i]);
                else if (_config->equalizer.mode == EQDoubleRainbow)
                {
                    // <gerstle> setting _pixels[_pixelCount - 1 - i] to _pixels[i] would work if half
                    // was actually half... so, run through both sides... :(
                    if (i < _half)
                        _pixels[i].setHSV(map(i, 0, _half, 0, 255), 255, _previousValues[i]);
                    else if (i > _half)
                        _pixels[i].setHSV(map((_pixelCount - i - 1), 0, (_pixelCount - _half - 1), 0, 255), 255, _previousValues[i]);
                }
                else
                    _pixels[i].setHSV(_config->equalizer.color.hue, _config->equalizer.color.saturation, _previousValues[i]);
            }
            else
                _pixels[i] = CRGB::Black;
        }
        // <cgerstle> this is the "ON" for rainbow
        else if (_config->equalizer.mode == EQSingleRainbow)
            _pixels[i].setHSV(map(i, 0, _pixelCount, 0, 255), 255, 255);
        else if (_config->equalizer.mode == EQDoubleRainbow)
        {
            if (i < _half)
                _pixels[i].setHSV(map(i, 0, _half, 0, 255), 255, 255);
            else if (i > _half)
                _pixels[i].setHSV(map((_pixelCount - i - 1), 0, (_pixelCount - _half - 1), 0, 255), 255, 255);
        }
        // <gerstle> this is the "ON" for not rainbow
        else
            _pixels[i] = _config->equalizer.color;
    }
}
