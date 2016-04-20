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
#include "../lw/LWUtils.h"

void ChaosLegMode::setup(LWConfigs *c, char const *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p)
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
    _directionTimer = random16(100, 5000);

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
        if (_currentTime > (_lastChangeTimer + _directionTimer))
        {
            _lastChangeTimer = _currentTime;
            _directionTimer = random16(100, 1500);
            int multiplier = random8(1, 4); 
            if (random8(2) == 0)
                _direction = -1 * multiplier;
            else
                _direction = 1 * multiplier;
        }

        _x += (_config->chaos.speed * _direction);
        _y += (_config->chaos.speed * _direction);


        for (int i = 0; i < _pixelCount; i++)
        {
            _perlinsTracker += 0.0005;
            //double adjuster = LWUtils.perlinNoise(_perlinsTracker + sin(i /  4), cos(_perlinsTracker), _perlinsTracker);
            double adjuster = inoise8(_x + i * 20, _y + i * 10, _perlinsTracker);

            if (_config->chaos.sparse)
            {
                // <cgerstle> this basically takes it to -1, 0, or 1 and creates a cool effect
                adjuster = map(adjuster, 0, 255, -1, 1.0);
            }
            else
                adjuster = (adjuster - 128) / (double) 128;

            _pixels[i].setHSV(_config->chaos.color.h + (adjuster * (double)_config->chaos.swing),
                              _config->chaos.color.s,
                              (adjuster * (double) 127) + 128);
        }
    }
}