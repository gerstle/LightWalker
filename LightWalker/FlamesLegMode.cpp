/*****************************************************************************
 * LegFlames.cpp
 *
 * This encapsulates the flames leg mode
 *
 * Copyright 2014 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include "FlamesLegMode.h"
#include "LWUtils.h"

void FlamesLegMode::setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p)
{
    if (_setup_complete)
        return;

    front_current_height = 10;
    front_next_height = 10;
    front_accent_low_end = 0;
    front_accent_high_end = 0;
    front_reset_accent = true;

    back_current_height = 10;
    back_next_height = 10;
    back_accent_low_end = 0;
    back_accent_high_end = 0;
    back_reset_accent = true;

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

void FlamesLegMode::frame()
{
    _currentTime = millis();

    if (stepDetected)
        _lastStepTime = _currentTime;

    if (_currentTime >= (_lastFrame + _config->flames.delay))
    {
        _lastFrame = _currentTime;

        _flame(0, _half, true, &front_current_height, &front_next_height, &front_accent_low_end, &front_accent_high_end, &front_reset_accent);
        _flame(_half, _pixelCount - _half, false, &back_current_height, &back_next_height, &back_accent_low_end, &back_accent_high_end, &back_reset_accent);
    }
}

void FlamesLegMode::_flame(int start, int count, bool invert, int *current_height, int *next_height, int *accent_low_end, int *accent_high_end, bool *reset_accent)
{
    int half = count / 2;
    int tmpIndex;

    // <gerstle> Set the next flame height to be within the range of the LEDs
    if (*current_height == *next_height)
    {
        *next_height += random(-8, 9);
        *next_height = max(*next_height, 0);
        *next_height = min(*next_height, count);
    }

    // <gerstle> current flame height should increment up to match next flame height
    if (*next_height <= *current_height)
        *current_height = *next_height;
    else
        *current_height = min(*current_height + 1, count);
        
    // <gerstle> 0 -> current_height: increment up to color
    for (int i = 0; i < *current_height; i++)
    {
        if (!invert)
            tmpIndex = start + i;
        else
            tmpIndex = start + count - 1 - i;

        _pixels[tmpIndex] += CHSV(random(0, 10), 255, 20);

        if (_currentTime > (_lastStepTime + _config->flames.stepMillis))
        {
            // <gerstle> keep them from staying static yellow
            if (_pixels[tmpIndex].g > 90)
                _pixels[tmpIndex].g -= random(0, 30);
        }
        else
            _pixels[tmpIndex].g = 0;

    }

    // <gerstle> if the current_height & next_height < half,
    // introduce some runners
    if ((*current_height < half) && (*next_height < half) &&(random(0, 3) <= 1))
    {
        if (*reset_accent)
        {
            *accent_low_end = random(*next_height + 3, count - 3);
            *accent_low_end = max(*accent_low_end, 0);

            *accent_high_end = random(*accent_low_end + 1, count - 1);
            *accent_high_end = min(*accent_high_end, count - 1);

            *reset_accent = false;
        }

        for (int i = *accent_low_end; ((i <= *accent_high_end) && (i <= (count - 1))); i++)
        {
            if (!invert)
                tmpIndex = start + i;
            else
                tmpIndex = start + count - 1 - i;

            _pixels[tmpIndex] += CHSV(random(32, 60), 255, random(20, 40));
        }
    }
    else
        *reset_accent = true;

    // <gerstle> >= current_height: slowly darken to black
    for (int i = *current_height; i < count; i++)
    {
        if (!invert)
            tmpIndex = start + i;
        else
            tmpIndex = start + count - 1 - i;

        _pixels[tmpIndex].fadeToBlackBy(random(1, 20));
    }
}
