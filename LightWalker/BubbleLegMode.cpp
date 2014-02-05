/*****************************************************************************
 * LegBubble.cpp
 *
 * This encapsulates the bubble leg mode
 *
 * Copyright 2014 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include "BubbleLegMode.h"

void BubbleLegMode::setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p)
{
    _leadingBubbleBottom = -10;
    _trailingBubbleBottom = _pixelCount + 10;

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

void BubbleLegMode::frame()
{
    int dice = 0;
    int leadingBubbleTop, trailingBubbleTop = 0;

    if (stepDetected)
    {
        _leadingBubbleBottom = _half;
        _trailingBubbleBottom = _half + 1 + (_half - _leadingBubbleBottom);
    }

    leadingBubbleTop = _leadingBubbleBottom - _config->bubble.width + 1;
    trailingBubbleTop = _trailingBubbleBottom + _config->bubble.width - 1;

    for (int i = 0; i < _pixelCount; i++)
    {
        dice = random(0, 100);

        if (((i <= _half) && (i <= _leadingBubbleBottom) && (i >= leadingBubbleTop)) ||
            ((i > _half) && (i >= _trailingBubbleBottom) && (i <= trailingBubbleTop)))
        {
            _pixels[i] = _config->bubble.bubbleColor;
        }
        else if (!_config->bubble.trail &&
                 (((i > _leadingBubbleBottom) && (i <= (_leadingBubbleBottom + _config->bubble.speed))) ||
                  ((i < _trailingBubbleBottom) && (i >= (_trailingBubbleBottom - _config->bubble.speed)))))
        {
            _pixels[i] = _config->bubble.backgroundColors[0];
        }
        else if (dice < 3)
        {
            _pixels[i] = _config->bubble.backgroundColors[dice];
        }
    }

    if ((_leadingBubbleBottom >= 0) || (_trailingBubbleBottom <= _pixelCount))
    {
        _leadingBubbleBottom -= _config->bubble.speed;
        _trailingBubbleBottom += _config->bubble.speed;
    }
}
