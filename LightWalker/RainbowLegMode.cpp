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
#include "LWUtils.h"

void RainbowLegMode::setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p)
{
    _lastStartHue = 0;
    _increment = (float)255/(float)count;

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
	double xyz[3];

    LWUtils.selectI2CChannels(_channel);
    _adxl->getAcceleration(xyz);
    _value = min(255, map(abs(xyz[0]) * 100, 0, 192, _config->rainbow.minValue, 255));

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
        _pixels[i].setHSV(map(i, 0, _pixelCount, 0, 255), 255, _value);
}

void RainbowLegMode::_doubleRainbow()
{
    for (int i = 0; i < _half; i++)
        _pixels[i].setHSV(map(i, 0, _half, 0, 255), 255, _value);

    // <gerstle> setting _pixels[_pixelCount - 1 - i] to _pixels[i] would work if half
    // was actually half... so, run through both sides... :(
    for (int i = _half; i < _pixelCount; i++)
        _pixels[i].setHSV(map((_pixelCount - i - 1), 0, (_pixelCount - _half - 1), 0, 255), 255, _value);
}

void RainbowLegMode::_rotate()
{
    if (_lastChangeTimer >= _config->rainbow.delay)
    {
        _lastChangeTimer = 0;

        // <gerstle>  this increment shows the entire hue spectrum on a leg
        // if you go less than that, not all the colors show all the time, kinda nice.
        for (int i = 0; i < _pixelCount; i++)
            _pixels[i].setHSV(_lastStartHue + (((float)i) * (_increment * .6)), 255, _value);

        // <gerstle> let it roll under
        _lastStartHue--;
    }
}

void RainbowLegMode::_rise()
{
    if (_lastChangeTimer >= _config->rainbow.delay)
    {
        _lastChangeTimer = 0;

        for (int i = 0; i < _half; i++)
            _pixels[i].setHSV(_lastStartHue + (((float)i) * _increment), 255, _value);

        // <gerstle> setting _pixels[_pixelCount - 1 - i] to _pixels[i] would work if half
        // was actually half... so, run through both sides... :(
        for (int i = _half; i < _pixelCount; i++)
            _pixels[i].setHSV(_lastStartHue + (((float)(_pixelCount - i - 1)) * _increment), 255, _value);

        // <gerstle> let it roll over
        _lastStartHue++;
    }
}
