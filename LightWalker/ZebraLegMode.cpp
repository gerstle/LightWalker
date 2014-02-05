/*****************************************************************************
 * LegZebra.cpp
 *
 * This encapsulates the zebra leg mode
 *
 * Copyright 2014 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include "ZebraLegMode.h"
#include "LWUtils.h"

void ZebraLegMode::setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p)
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

    // <gerstle> paint the initial canvas
    byte stage = _startStage;
    for (int i = 0; i < _half; i++) // <gerstle> down
        paintPixel(&stage, i);

    stage = _startStage;
    for (int i = _pixelCount - 1; i >= _half; i--) // <gerstle> down
        paintPixel(&stage, i);

    _setup_complete = true;
}

void ZebraLegMode::frame()
{
    if (stepDetected)
    {
        if (_channel == ADXL_ONE)
            Serial.println("------------------------STEP!");
        _lastStepTimer = 0;
        _lastSlowTimer = 0;
        _frames = 1;
    }

    if (_lastStepTimer < 8000)
    {
        byte stage = _startStage;
        // for (int i = _half - 1; i >= 0; i--) // <gerstle> up
        for (int i = 0; i < _half; i++) // <gerstle> down
            paintPixel(&stage, i);

        stage = _startStage;
        //for (int i = _half; i < _pixelCount; i++) // <gerstle> up
        for (int i = _pixelCount - 1; i >= _half; i--) // <gerstle> down
            paintPixel(&stage, i);

        if (_lastSlowTimer >= 400)
        {
            _lastSlowTimer = 0;
            _frames++;
        }

        _frame++;
        if (_frame >= _frames)
        {
            _frame = 0;
            if (_startStage <= 0)
                _startStage = (ZEBRA_LIGHTS - 1);
            else
                _startStage--;
        }
    }
}

void ZebraLegMode::paintPixel(byte (*stage), int index)
{
    if ((*stage) == 0)
        LWUtils.setTransitionColor(&(_pixels[index]), _frame, _frames, _config->zebra.colorTwo, _config->zebra.colorOne);
    else if (((*stage) == 1) || ((*stage) == 2) || ((*stage) == 3))
        _pixels[index] = _config->zebra.colorTwo;
    else if ((*stage) == 4)
        LWUtils.setTransitionColor(&(_pixels[index]), _frame, _frames, _config->zebra.colorOne, _config->zebra.colorTwo);
    else if ((*stage) == 5)
        _pixels[index] = _config->zebra.colorOne;

    (*stage)++;
    if ((*stage) >= ZEBRA_LIGHTS)
        (*stage) = 0;
}
