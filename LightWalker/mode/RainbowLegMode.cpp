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
#include "../lw/LWUtils.h"

void RainbowLegMode::setup(LWConfigs *c, char const *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p)
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

    _perlinZ = (double) random8() / (double) 25500;

    _setup_complete = true;
}

void RainbowLegMode::frame()
{
    _halfMin = (double)_config->rainbow.minValue / 2;
    _currentTime = millis();

    switch (_config->rainbow.mode)
    {
        case SingleRainbow:
        case DoubleRainbow:
            if (_currentTime > (_lastNoiseTimer + 50))
            {
                _lastNoiseTimer = _currentTime;
                _perlinZ += 0.001;
            }
            else
            {
                // <gerstle> reset back to previous value so the calcuations work
                // out to be exactly the same again.
                _perlinZ -= (float)_pixelCount * 0.0005;
            }
            break;
        default:
            break;
    }

    if (stepDetected)
        _lastStepTime = _currentTime;

    switch (_config->rainbow.mode)
    {
        case SingleRainbow:
            _singleRainbow();
            break;
        case DoubleRainbow:
            _doubleRainbow();
            break;
        case Rotate:
            _rotate();
            break;
        case Rise:
            _rise();
            break;
    }
}

void RainbowLegMode::_singleRainbow()
{
    for (int i = 0; i < _pixelCount; i++)
        _pixels[i].setHSV(map(i, 0, _pixelCount, 0, 255), 255, _getValue(i));
}

void RainbowLegMode::_doubleRainbow()
{
    for (int i = 0; i < _half; i++)
        _pixels[i].setHSV(map(i, 0, _half, 0, 255), 255, _getValue(i));

    // <gerstle> setting _pixels[_pixelCount - 1 - i] to _pixels[i] would work if half
    // was actually half... so, run through both sides... :(
    for (int i = _half; i < _pixelCount; i++)
        _pixels[i].setHSV(map((_pixelCount - i - 1), 0, (_pixelCount - _half - 1), 0, 255), 255, _getValue(i));
}

void RainbowLegMode::_rotate()
{
    if (_currentTime > (_lastChangeTimer + _config->rainbow.delay))
    {
        _lastChangeTimer = _currentTime;
        _perlinZ += 0.001;

        // <gerstle>  this increment shows the entire hue spectrum on a leg
        // if you go less than that, not all the colors show all the time, kinda nice.
        for (int i = 0; i < _pixelCount; i++)
            _pixels[i].setHSV(_lastStartHue + (((float)i) * (_increment * .6)), 255, _getValue(i));

        // <gerstle> let it roll under
        _lastStartHue--;
    }
}

void RainbowLegMode::_rise()
{
    if (_currentTime > (_lastChangeTimer + _config->rainbow.delay))
    {
        _lastChangeTimer = _currentTime;
        _perlinZ += 0.001;

        for (int i = 0; i < _half; i++)
            _pixels[i].setHSV(_lastStartHue + (((float)i) * _increment), 255, _getValue(i));

        // <gerstle> setting _pixels[_pixelCount - 1 - i] to _pixels[i] would work if half
        // was actually half... so, run through both sides... :(
        for (int i = _half; i < _pixelCount; i++)
            _pixels[i].setHSV(_lastStartHue + (((float)(_pixelCount - i - 1)) * _increment), 255, _getValue(i));

        // <gerstle> let it roll over
        _lastStartHue++;
    }
}

byte RainbowLegMode::_getValue(int i)
{
    float value;

    if (_currentTime > (_lastStepTime + 1000))
    {
        _perlinZ += 0.0015;
        double noiseModifier = LWUtils.perlinNoise(_perlinZ + sin(i /  4), cos(_perlinZ), _perlinZ);
        //return (noiseModifier * (double)_halfMin) + (double)_halfMin * 1.5;
        return map((noiseModifier * (double)127) + (double)128, 0, 255, 30, (double)_config->main.maxBrightness * (double) 0.6);
    }

    // <cgerstle> value based on location... brighter at the bottom
    if (i < _half)
        value = ((float) i / (float) _half) * 255;
    else
        value = ((float) (_pixelCount - 1 - i) / (float) (_pixelCount - _half)) * 255;
    value = max(_config->rainbow.minValue, value);

    // <gerstle> now dim over time
    value = max(_config->rainbow.minValue, value - (((float)(_currentTime - _lastStepTime) / (float)1000) * (float)(value - _config->rainbow.minValue)));

    return value;
}
