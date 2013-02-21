/*****************************************************************************
 * Leg.cpp
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#include "Leg.h"

// ----------------------------------------------------------------------------
// Leg Class
// ----------------------------------------------------------------------------
Leg::Leg()
{
}

Leg::Leg(String n, int pin, int pixels, int x, WalkingModeEnum mode)
{
    if (DEBUG)
    {
        Serial.print("initing..."); Serial.print(n); Serial.print(" - "); Serial.print(pin); Serial.print(" - "); Serial.println(pixels);
    }

    name = n;
    trigger_pin = pin;
    pixel_count = pixels;
    status = Initialized;
    _lightMode = None;
    _lightModeChangeTime = millis();

    _sparkle_fade_rate = SPARKLE_FOOT_DOWN_FADE_STEP;

    sparkle_fade_on = true;
    sparkle_foot_down_fade_step = SPARKLE_FOOT_DOWN_FADE_STEP;
    sparkle_foot_up_fade_step = SPARKLE_FOOT_UP_FADE_STEP;

    int half = pixel_count / 2;

    if ((pixel_count % 2) > 0)
        lower_foot_border = half - 2;
    else
        lower_foot_border = half - 3;
    upper_foot_border = half + 2;

    off();
}

void Leg::sparkle_footdown()
{
    status = Down;
    if (DEBUG)
    {
        Serial.print(name); Serial.println(" -> down");
        Serial.print("fade rate to -> "); Serial.println(sparkle_foot_down_fade_step);
    }

    _sparkle_fade_rate = sparkle_foot_down_fade_step;
    if (_sparkle_fade_rate <= 0)
        _sparkle_fade_rate = 2;
    _sparkle_flash();
}

void Leg::sparkle_footup()
{
    if (DEBUG)
    {
        Serial.print(name); Serial.println(" -> up");
        Serial.print("fade rate to -> "); Serial.println(sparkle_foot_up_fade_step);
    }

    _sparkle_fade_rate = sparkle_foot_up_fade_step;
    if (_sparkle_fade_rate <= 0)
        _sparkle_fade_rate = 2;
    status = Up;
    sparkle_sameStatus();
}

void Leg::sparkle_sameStatus()
{
//     Serial.print("leg "); Serial.print(name); Serial.println(" -> same");
    unsigned long currentTime = millis();
    switch (_lightMode)
    {
        case Flash:
                _sparkle_sparkle();
            break;

        case SparkleSparkle:
            if (currentTime <= (_lightModeChangeTime + SPARKLE_INTERVAL))
                _sparkle_sparkle();
            else
                _sparkle_fade();
            break;

        case Fade:
            _sparkle_fade();
            break;

        case Off:
            off();
            break;
    }
}

void Leg::_sparkle_flash()
{
    if (DEBUG)
    {
        Serial.println("_sparkle_flashing " + name);
        Serial.print("    pixel_count:"); Serial.println(pixel_count);
        Serial.print("    lower foot border:"); Serial.println(lower_foot_border);
        Serial.print("    upper foot border:"); Serial.println(upper_foot_border);
    }

    for (int i = 0; i < pixel_count; i++)
    {
        if ((i < lower_foot_border) || (i > upper_foot_border))
            _setPixel(i, COLOR_BLACK, 0x00);
        else
            _setPixel(i, COLOR_WHITE, 0x00);
    }
    _setLightMode(Flash);
}

void Leg::_sparkle_sparkle()
{
    //Serial.println("sparkling " + name);
    byte level;
    int distance;
    for (int i = 0; i < lower_foot_border; i++)
    {
        distance = lower_foot_border - i;
        level = byte(random(0, ((LWConfigs.main.maxBrightness/(lower_foot_border * 3)) * (i + 1))));
        _pixels[i] = RGB(level, level, level);
    }

    for (int i = upper_foot_border + 1; i < pixel_count; i++)
    {
        // <gerstle> white sparkle
        level = byte(random(0, ((LWConfigs.main.maxBrightness/(lower_foot_border * 3)) * (lower_foot_border - (i - upper_foot_border) + 1))));
        _pixels[i].r = level;
        _pixels[i].g = level;
        _pixels[i].b = level;

        // <gerstle> random color sparkle
//         _pixels[i].r = byte(random(0, ((max_brightness/(lower_foot_border * 3)) * (lower_foot_border - (i - upper_foot_border) + 1))));
//         _pixels[i].g = byte(random(0, ((max_brightness/(lower_foot_border * 3)) * (lower_foot_border - (i - upper_foot_border) + 1))));
//         _pixels[i].b = byte(random(0, ((max_brightness/(lower_foot_border * 3)) * (lower_foot_border - (i - upper_foot_border) + 1))));
    }

    _displayPixels();
    _setLightMode(SparkleSparkle);
}

void Leg::_sparkle_fade()
{
    if (sparkle_fade_on)
    {
//         Serial.println("fading " + name);
        bool still_fading = false;

        for (int i = 0; i < pixel_count; i++)
            if (((_pixels[i].r - _sparkle_fade_rate) > 0x00) ||
                ((_pixels[i].g - _sparkle_fade_rate) > 0x00) ||
                ((_pixels[i].b - _sparkle_fade_rate) > 0x00))
            {
                
//                 Serial.print("_sparklefade "); Serial.print(name); Serial.print(" to: "); Serial.print(_pixels[i].r - _sparkle_fade_rate); Serial.print(" - "); Serial.print(_pixels[i].g - _sparkle_fade_rate); Serial.print(" - "); Serial.println(_pixels[i].b - _sparkle_fade_rate);
                _setPixel(i, _pixels[i], _sparkle_fade_rate);
                still_fading = true;
            }
            else
                _setPixel(i, COLOR_BLACK, 0x00);

            if (still_fading)
                _setLightMode(Fade);
            else
                off();
    }
    else
        off();
}

void Leg::pulse_pulse()
{
    unsigned long currentTime = millis();

    if (currentTime >= (_lightModeChangeTime + _pulse_length))
    {
        _lightModeChangeTime = currentTime;
        _pulse_isDimming = !_pulse_isDimming;

        _pulse_length = random(LWConfigs.pulse.minPulseTime, LWConfigs.pulse.maxPulseTime);
    }

    
    int value = 0;
    
    if (_pulse_isDimming)
        value = _pulse_length - (currentTime - _lightModeChangeTime);
    else
        value = currentTime - _lightModeChangeTime;
    float brightness = ((float)map(value, 0, _pulse_length, 0, 100) / 100);

    RGB newColor;
    newColor.r = byte((float)LWConfigs.pulse.color.r * (brightness * brightness * brightness));
    newColor.g = byte((float)LWConfigs.pulse.color.g * (brightness * brightness * brightness));
    newColor.b = byte((float)LWConfigs.pulse.color.b * (brightness * brightness * brightness));

    //Serial.print(newColor.r); Serial.print(" - "); Serial.print(newColor.g); Serial.print(" - "); Serial.println(newColor.b);

    for (int i = 0; i < pixel_count; i++)
        _setPixel(i, newColor, 0);

}

void Leg::_displayPixels()
{
    for (int i = 0; i < pixel_count; i++)
        LWUtils.sendColor(_pixels[i]);
}

void Leg::_setPixel(int i, RGB color, byte dimmer)
{
    if ((i == 0) && false)
    {
        Serial.print("_setPixel before:: "); Serial.print(color.r); Serial.print(" - "); Serial.print(color.g); Serial.print(" - "); Serial.print(color.b); Serial.print(" dim: "); Serial.println(dimmer);
    }

    if (color.r > dimmer)
        _pixels[i].r = color.r - dimmer;
    else
        _pixels[i].r = 0;

    if (color.g > dimmer)
        _pixels[i].g = color.g - dimmer;
    else
        _pixels[i].g = 0;

    if (color.b > dimmer)
        _pixels[i].b = color.b - dimmer;
    else
        _pixels[i].b = 0;

    if ((i == 0) && false)
    {
        Serial.print("_setPixel:: "); Serial.print(_pixels[i].r); Serial.print(" - "); Serial.print(_pixels[i].g); Serial.print(" - "); Serial.print(_pixels[i].b); Serial.print(" dim: "); Serial.println(dimmer);
    }

    // <gerstle> if any of the colors are over LWConfigs.main.maxBrightness
    // step down all the pixels. hopefully that doesn't change
    // the color
    // do the offending pixel last!
    if (_pixels[i].r > LWConfigs.main.maxBrightness)
    {
        _applyMaxBrightness(&_pixels[i].g, (float)_pixels[i].r, i);
        _applyMaxBrightness(&_pixels[i].b, (float)_pixels[i].r, i);
        _applyMaxBrightness(&_pixels[i].r, (float)_pixels[i].r, i);
    }

    if (_pixels[i].g > LWConfigs.main.maxBrightness)
    {
        _applyMaxBrightness(&_pixels[i].r, (float)_pixels[i].g, i);
        _applyMaxBrightness(&_pixels[i].b, (float)_pixels[i].g, i);
        _applyMaxBrightness(&_pixels[i].g, (float)_pixels[i].g, i);
    }

    if (_pixels[i].b > LWConfigs.main.maxBrightness)
    {
        _applyMaxBrightness(&_pixels[i].r, (float)_pixels[i].b, i);
        _applyMaxBrightness(&_pixels[i].g, (float)_pixels[i].b, i);
        _applyMaxBrightness(&_pixels[i].b, (float)_pixels[i].b, i);
    }

    if ((i == 0) && false)
    {
        Serial.print("_setPixel:: "); Serial.print(_pixels[i].r); Serial.print(" - "); Serial.print(_pixels[i].g); Serial.print(" - "); Serial.print(_pixels[i].b); Serial.print(" dim: "); Serial.println(dimmer);
    }

    LWUtils.sendColor(_pixels[i]);
}

void Leg::_applyMaxBrightness(byte *pixel, float value, int i)
{
    float diffPercentage = (float)1 - ((value - (float)LWConfigs.main.maxBrightness)/value);
    *pixel = byte((float)(*pixel) * diffPercentage);
//     Serial.print("                       "); Serial.println(LWConfigs.main.maxBrightness); Serial.print("\t"); Serial.print(*pixel);
}

void Leg::off()
{
    for (int i = 0; i < pixel_count; i++)
        _setPixel(i, COLOR_BLACK, 0x00);
    _setLightMode(Off);
}

void Leg::_setLightMode(LightModeEnum mode)
{
    if (_lightMode != mode)
        _lightModeChangeTime = millis();
    _lightMode = mode;
}

void Leg::setWalkingMode(WalkingModeEnum mode)
{
    _walkingMode = mode;
    _lightMode = Off;

    _lightModeChangeTime = millis();

    switch (_walkingMode)
    {
        case Pulse:
            _pulse_isDimming = false;
            _pulse_length = random(LWConfigs.pulse.minPulseTime, LWConfigs.pulse.maxPulseTime);

            break;
    }
}
