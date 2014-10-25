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

    //_perlinZ = (double) random8() / (double) 25500;
    _perlinsTracker = random16();
    _x = random16();
    _y = random16();
    _lastChangeTimer = millis();

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

    if (currentTime > (_lastChangeTimer + 400))
    {
        _lastChangeTimer = currentTime;
        if (random8(2) == 0)
            _direction = -1;
        else
            _direction = 1;
    }

    _x += (2 * _direction);
    _y += (2 * _direction);

    for (i = 0; i < _pixelCount; i++)
    {
        // <cgerstle> this is the "OFF"
        if (((i <= _half) && (i < lower_threshold)) ||
            ((i > _half) && (i > upper_threshold)))
        {
            if (_config->equalizer.minValue > 0)
            {
                _perlinsTracker += 0.0005;
                byte value = inoise8(_x + i * 20, _y + i * 10, _perlinsTracker);
                // <cgerstle> 0-255, scale down a bit
                value = map(value, 20, 255, 0, _config->equalizer.minValue);

                if (_config->equalizer.mode == EQSingleRainbow)
                    _pixels[i].setHSV(map(i, 0, _pixelCount, 0, 255), 255, value);
                else if (_config->equalizer.mode == EQDoubleRainbow)
                {
                    // <gerstle> setting _pixels[_pixelCount - 1 - i] to _pixels[i] would work if half
                    // was actually half... so, run through both sides... :(
                    if (i < _half)
                        _pixels[i].setHSV(map(i, 0, _half, 0, 255), 255, value);
                    else if (i > _half)
                        _pixels[i].setHSV(map((_pixelCount - i - 1), 0, (_pixelCount - _half - 1), 0, 255), 255, value);
                }
                else
                    _pixels[i].setHSV(_config->equalizer.color.hue, _config->equalizer.color.saturation, value);
            }
            else
                _pixels[i] = CRGB::Black;
        }
        // <cgerstle> this is the "ON" for rainbow
        else if (_config->equalizer.mode == EQSingleRainbow)
            _pixels[i].setHSV(map(i, 0, _pixelCount, 0, 255), 255, 255);
        else if (_config->equalizer.mode == EQDoubleRainbow)
        {
            if (i <= _half)
                _pixels[i].setHSV(map(i, 0, _half, 0, 255), 255, 255);
            else if (i > _half)
                _pixels[i].setHSV(map((_pixelCount - i - 1), 0, (_pixelCount - _half - 1), 0, 255), 255, 255);
        }
        // <gerstle> this is the "ON" for not rainbow
        else
            _pixels[i] = _config->equalizer.color;
    }
}
