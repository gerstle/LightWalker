/*****************************************************************************
 * LegSparkle.cpp
 *
 * This encapsulates the sparkle leg mode
 *
 * Copyright 2014 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include "SparkleLegMode.h"

void SparkleLegMode::setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p)
{
    _state = Off;
    _lastChangeTimer = 0;

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

    if ((_pixelCount % 2) > 0)
        _lowerFootBorder = _half - 4;
    else
        _lowerFootBorder = _half - 5;
    _upperFootBorder = _half + 4;

//     Serial.print(_legName); Serial.println(" with:");
//     Serial.print("    pixel count: "); Serial.println(_pixelCount);
//     Serial.print("    half: "); Serial.println(_half);

    _setup_complete = true;
}

void SparkleLegMode::frame()
{
//     if (_channel == ADXL_ONE)
//         Serial.println("sparklelegmode::frame");
    if (stepDetected)
    {
//         if (_channel == ADXL_ONE)
//             Serial.println("frame::footdown");
        _footDown();
    }
    else
    {
//         if (_channel == ADXL_ONE)
//             Serial.println("frame::same");
        _sameStatus();
    }
}

void SparkleLegMode::_setState(LightStateEnum newState)
{
    if (_state != newState)
    {
//         if (_channel == ADXL_ONE)
//             Serial.println("resetting timer");
        _lastChangeTimer = 0;
    }

//     if (_channel == ADXL_ONE)
//     {
//         Serial.print("_setState: "); Serial.println(newState);
//     }
    _state = newState;
}

void SparkleLegMode::_sameStatus()
{
//     if (_channel == ADXL_ONE)
//     {
//         Serial.print(_legName); Serial.print(" -> same -> "); Serial.println(_state);
//     }

    switch (_state)
    {
        // <gerstle> flash really just affects the foot, so it's just
        // sparkle with a different foot color
        case Flash:
        case SparkleSparkle:
            _sparkle();
            break;

        case Fade:
            _fade();
            break;

        case Off:
            _shimmer();
            break;
    }
}

// <gerstle> shimmer happens all the time.
void SparkleLegMode::_shimmer()
{
//     if (_channel == ADXL_ONE)
//     {
//         Serial.print("shimmering "); Serial.println(_legName);
//     }

    float brightness = 0;
    int dice = 0;

    for (int i = 0; i < _pixelCount; i++)
    {
        dice = random(0, 100);
        // <cgerstle> leave the middle ones on bright...
        if ((i <= (_half + 1)) && (i >= (_half - 1)))
        {
            _pixels[i] = _config->sparkle.sparkleColor;
        }
        // <cgerstle> sparkle the rest if minbrightness is on
        else if (dice == 0)
        {
            _pixels[i].setHSV(_config->sparkle.sparkleColor.hue, _config->sparkle.sparkleColor.saturation, max(_config->sparkle.minValue, (_config->sparkle.sparkleColor.value - random(75, 125)) / 2));
        }
    }

    _setState(Off);
}

// <gerstle> footdown -> flash -> sparkle -> fade -> off/shimmer
void SparkleLegMode::_footDown()
{
//     if (_channel == ADXL_ONE)
//     {
//         Serial.print("step! "); Serial.println(_legName);
//     }
    _flash();
}

// <gerstle> footdown -> flash -> sparkle -> fade -> off/shimmer
void SparkleLegMode::_flash()
{
//     if (_channel == ADXL_ONE)
//     {
//         Serial.print("flashing "); Serial.println(_legName);
//     }

    for (int i = 0; i < _pixelCount; i++)
    {
        if ((i < _lowerFootBorder) || (i > _upperFootBorder))
            _pixels[i] = CRGB::Black;
        else
        {
            // <gerstle> Let's make sure this is cranked up...
            _pixels[i].setHSV(_config->sparkle.footFlashColor.hue, _config->sparkle.footFlashColor.saturation, 255);
        }
    }
    _setState(Flash);
}

// <gerstle> footdown -> flash -> sparkle -> fade -> off/shimmer
void SparkleLegMode::_sparkle()
{
//     if (_channel == ADXL_ONE)
//     {
//         Serial.print("\tsparkling "); Serial.println(_legName);
//     }

    float sparkleValue;
    float rand = 0;

    // leading leg
    for (int i = 0; i < _lowerFootBorder; i++)
    {
        sparkleValue = ((float) i / (float) _half);
        rand = (float)random(0, min(4, sparkleValue * 100)) / 100;
        sparkleValue = (sparkleValue - rand) * 255;
        //Serial.print(i); Serial.print("\t"); Serial.print(sparkleValue); Serial.print("\t"); Serial.println(rand);
        _pixels[i].setHSV(_config->sparkle.sparkleColor.hue, _config->sparkle.sparkleColor.saturation, sparkleValue);
    }

    //foot
    CHSV *footColor = &_config->sparkle.footFlashColor;
    if (_lastChangeTimer >= _config->sparkle.flashLength)
        footColor = &_config->sparkle.sparkleColor;

    for (int i = _lowerFootBorder; i <= _upperFootBorder; i++)
        _pixels[i] = *footColor;

    // trailing leg
    for (int i = _upperFootBorder + 1; i < _pixelCount; i++)
    {
        sparkleValue = ((float) (_pixelCount - 1 - i) / (float) _half);
        if (random(0, 30) == 0)
            rand = (float)random(0, min(20, sparkleValue * 100)) / 100;
        sparkleValue = (sparkleValue - rand) * 256;
        //Serial.print(i); Serial.print("\t"); Serial.print(sparkleValue); Serial.print("\t"); Serial.println(rand);
        _pixels[i].setHSV(_config->sparkle.sparkleColor.hue, _config->sparkle.sparkleColor.saturation, sparkleValue);
    }

    // <gerstle> if we're past our sparkle time, start fading
    if (_lastChangeTimer >= _config->sparkle.sparkleLength)
    {
//         for (int i = 0; i < _pixelCount; i++)
//             if ((i >= _lowerFootBorder) && (i <= _upperFootBorder))
        for (int i = _lowerFootBorder; i <= _upperFootBorder; i++)
                _pixels[i] = _config->sparkle.sparkleColor;
//         Serial.println("setting to fade");
        _setState(Fade);
    }
    else
    {
//         Serial.println("setting to sparkle");
        _setState(SparkleSparkle);
    }
}

// <gerstle> footdown -> flash -> sparkle -> fade -> off/shimmer
void SparkleLegMode::_fade()
{
//     if (_channel == ADXL_ONE)
//     {
//         Serial.print("\t\tfading "); Serial.println(_legName);
//     }

    int still_fading = 0;

    for (int i = 0; i < _pixelCount; i++)
        if (((_pixels[i].r - _config->sparkle.fadeRate) > 0) ||
            ((_pixels[i].g - _config->sparkle.fadeRate) > 0) ||
            ((_pixels[i].b - _config->sparkle.fadeRate) > 0))
        {
            _pixels[i].fadeToBlackBy(_config->sparkle.fadeRate);
            still_fading++;
        }

        if ((_config->sparkle.minValue > 0) && (still_fading > (_pixelCount / 2)))
            _setState(Fade);
        else if ((_config->sparkle.minValue == 0) && (still_fading))
            _setState(Fade);
        else
            _setState(Off);
}
