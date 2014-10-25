/*****************************************************************************
 * LegChaos.cpp
 *
 * This encapsulates the zebra leg mode
 *
 * Copyright 2014 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include<FastLED.h>
#include "ChaosLegMode.h"
#include "LWUtils.h"

void ChaosLegMode::setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p)
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

    _perlinsTracker = random16();
    _x = random16();
    _y = random16();
    _hue = 0;

    _setup_complete = true;
}

void ChaosLegMode::frame()
{
    _currentTime = millis();

    if (stepDetected)
        _lastStepTime = _currentTime;

    if (_currentTime <= (_lastStepTime + _config->chaos.stepLength))
    {
            _hue += 32;
            byte value = map(_currentTime, _lastStepTime, _lastStepTime + _config->chaos.stepLength,
                             255, min(255, _config->chaos.minValue + _config->chaos.minValue / 2));
            for (int i = 0; i < _pixelCount; i++)
                _pixels[i].setHSV(_hue, 255, value);
    }
    else
    {
        if (_currentTime > (_timer + _config->chaos.delay))
        {
            _timer = _currentTime;
            _x += 4;
            _y -= 4;
            for (int i = 0; i < _pixelCount; i++)
            {
                _perlinsTracker += 0.0005;
                //double adjuster = LWUtils.perlinNoise(_perlinsTracker + sin(i /  4), cos(_perlinsTracker), _perlinsTracker);
                double adjuster = inoise8(_x + i * 20, _y + i * 10, _perlinsTracker);
                _pixels[i].setHSV(adjuster,
                                  _config->chaos.color.s,
                                  adjuster);
                //_pixels[i].setHSV(_config->chaos.color.h + (adjuster * (double)_config->chaos.swing),
                                  //_config->chaos.color.s,
                                  //(adjuster * (double) 127) + 128);
                //_pixels[i].setHSV(0, 255, 100);
            }
        }
    }
}
