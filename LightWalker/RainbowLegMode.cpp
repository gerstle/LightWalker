/*****************************************************************************
 * LegRainbow.cpp
 *
 * This encapsulates the rainbow leg mode
 *
 * Copyright 2014 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include "RainbowLegMode.h"

void RainbowLegMode::setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p)
{
    _lastStartHue = 0;
    _increment = (float)255/(float)LED_COUNT;

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

void RainbowLegMode::frame()
{
    switch (_config->rainbow.mode)
    {
        case singleRainbow:
            _singleRainbow();
            break;
        case doubleRainbow:
            _doubleRainbow();
            break;
        case rotate:
            _rotate();
            break;
        case rise:
            _rise();
            break;
    }
}

void RainbowLegMode::_singleRainbow()
{
    for (int i = 0; i < _pixelCount; i++)
        _pixels[i].setHSV(map(i, 0, _pixelCount, 0, 255), 255, _config->rainbow.minValue);
}

void RainbowLegMode::_doubleRainbow()
{
    for (int i = 0; i < _half; i++)
        _pixels[i].setHSV(map(i, 0, _half, 0, 255), 255, _config->rainbow.minValue);

    for (int i = _half; i < _pixelCount; i++)
        _pixels[i].setHSV(map((_pixelCount - i - 1), 0, (_pixelCount - _half - 1), 0, 255), 255, _config->rainbow.minValue);
}

void RainbowLegMode::_rotate()
{
    if (_lastChangeTimer >= _config->rainbow.delay)
    {
        _lastChangeTimer = 0;

        int _counter = _pixelCount;
        while (_counter--)
            _pixels[_counter] = CHSV(_lastStartHue + (((float)_counter) * _increment), 255, _config->rainbow.minValue);

        _lastStartHue++;
        if (_lastStartHue > 255)
            _lastStartHue = 0;
    }
}

void RainbowLegMode::_rise()
{
}
