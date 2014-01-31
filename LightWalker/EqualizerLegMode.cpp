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

    _setup_complete = true;
}

// <cgerstle> eqLevel is 0 -> 100
void EqualizerLegMode::frame()
{
    if (_config->equalizer.allLights)
    {
        CHSV tmpColor = CHSV(_config->equalizer.color.hue, _config->equalizer.color.saturation, map(eqLevel, 0, 100, _config->equalizer.minValue, 255));
        for (int i = 0; i < _pixelCount; i++)
            _pixels[i] = tmpColor;
    }
    else
    {
        int i = 0;
        int lower_threshold = _half - (eqLevel * _half);
        int upper_threshold = (eqLevel * _half) + _half;
        int rainbow_color_offset = 2;

        for (i = 0; i < _pixelCount; i++)
        {
            // <cgerstle> this is the "OFF"
            if (((i < _half) && (i < lower_threshold)) ||
                ((i > _half) && (i > upper_threshold)))
            {
                if (_config->equalizer.minValue > 0)
                {
                    if (_config->equalizer.rainbow)
                        _pixels[i].setHSV(map(i, 0, _pixelCount, 0, 255), 255, _config->equalizer.minValue);
                    else
                        _pixels[i].setHSV(_config->equalizer.color.hue, _config->equalizer.color.saturation, _config->equalizer.minValue);
                }
                else
                    _pixels[i] = CRGB::Black;
            }
            // <cgerstle> this is the "ON" for rainbow
            else if (_config->equalizer.rainbow)
                _pixels[i].setHSV(map(i, 0, _pixelCount, 0, 255), 255, 255);
            // <gerstle> this is the "ON" for not rainbow
            else
                _pixels[i] = _config->equalizer.color;
        }
    }
}
