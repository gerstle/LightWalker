/*****************************************************************************
 * LegGravity.cpp
 *
 * This encapsulates the gravity leg mode
 *
 * Copyright 2014 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include "../modes/GravityLegMode.h"

#include "../lw/LWUtils.h"

void GravityLegMode::setup(config::LWConfigs *c, char const *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p, int baseIndex)
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
    _baseIndex = baseIndex;

    _setup_complete = true;
}

void GravityLegMode::frame()
{
	double xyz[3];

	// TODO
    // LWUtils.selectI2CChannels(_channel);
    _adxl->getAcceleration(xyz);

    if (_indexOne == 2)
    {
        if (xyz[_indexOne] > 0)
            _pixels[0].r = map(abs(xyz[_indexOne]) * 100, 0, 192, _config->gravity.minValue, 255);
        else
            _pixels[0].r = 0;
    }
    else
        _pixels[0].r = map(abs(xyz[_indexOne]) * 100, 0, 192, _config->gravity.minValue, 255);

    if (_indexTwo == 2)
    {
        if (xyz[_indexTwo] > 0)
            _pixels[0].g = map(abs(xyz[_indexOne]) * 100, 0, 192, _config->gravity.minValue, 255);
        else
            _pixels[0].g = 0;
    }
    else
        _pixels[0].g = map(abs(xyz[_indexTwo]) * 100, 0, 192, _config->gravity.minValue, 255);

    if (_indexThree == 2)
    {
        if (xyz[_indexThree] > 0)
            _pixels[0].b = map(abs(xyz[_indexThree]) * 100, 0, 192, _config->gravity.minValue, 255);
        else
            _pixels[0].b = 0;
    }
    else
        _pixels[0].b = map(abs(xyz[_indexThree]) * 100, 0, 192, _config->gravity.minValue, 255);


    if (_config->gravity.rotate)
    {
        if ((abs(xyz[0]) > 0.9) && _canXSwitch)
        {
            if (_indexOne == 0)
            {
                    _indexOne = 2;
                    _indexThree = 0;
                    _indexTwo = 1;
            }
            else if (_indexTwo == 0)
            {
                    _indexOne = 0;
                    _indexThree = 1;
                    _indexTwo = 2;
            }
            else if (_indexThree == 0)
            {
                    _indexOne = 1;
                    _indexThree = 2;
                    _indexTwo = 0;
            }
            _canXSwitch = false;
        }

        if ((abs(xyz[1]) > 240) && _canYSwitch)
        {
            if (_indexOne == 1)
            {
                    _indexOne = 2;
                    _indexThree = 0;
                    _indexTwo = 1;
            }
            else if (_indexTwo == 1)
            {
                    _indexOne = 0;
                    _indexThree = 1;
                    _indexTwo = 2;
            }
            else if (_indexThree == 1)
            {
                    _indexOne = 1;
                    _indexThree = 2;
                    _indexTwo = 0;
            }
            _canYSwitch = false;
        }

        if ((abs(xyz[0]) < 0.3) && (_lastXSwitch >= 600))
        {
            _lastXSwitch = _lastXSwitch - 600;
            _canXSwitch = true;
        }

        if ((abs(xyz[1]) < 0.3) && (_lastYSwitch >= 600))
        {
            _lastYSwitch = _lastYSwitch - 600;
            _canYSwitch = true;
        }
    }

    for (int i = 1; i < _pixelCount; i++)
    {
        if ((_pixels[0].r > 10) && (random(0, 20) == 0))
            _pixels[i].r = _pixels[0].r - random(0, 10);
        else
            _pixels[i].r = _pixels[0].r;

        if ((_pixels[0].g > 10) && (random(0, 20) == 0))
            _pixels[i].g = _pixels[0].g - random(0, 10);
        else
            _pixels[i].g = _pixels[0].g;

        if ((_pixels[0].b > 10) && (random(0, 20) == 0))
            _pixels[i].b = _pixels[0].b - random(0, 10);
        else
            _pixels[i].b = _pixels[0].b;

    }
}
