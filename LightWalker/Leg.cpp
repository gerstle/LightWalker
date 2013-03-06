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
    name = "one";
}

void Leg::Init(String n, int pin, WalkingModeEnum mode)
{
    name = n;
    trigger_pin = pin;
    status = Initialized;
    _lightMode = None;
    _lightModeChangeTime = millis();

    _sparkle_fade_rate = LWConfigs.sparkle.footDownFadeRate;

    sparkle_fade_on = true;

    int half = PIXELS_PER_LEG / 2;

    if ((PIXELS_PER_LEG % 2) > 0)
        lower_foot_border = half - 2;
    else
        lower_foot_border = half - 3;
    upper_foot_border = half + 2;
}

void Leg::sparkle_footdown()
{
    status = Down;
    if (DEBUG)
    {
        Serial.print(name); Serial.println(" -> down");
        Serial.print("fade rate to -> "); Serial.println(LWConfigs.sparkle.footDownFadeRate);
    }

    _sparkle_fade_rate = LWConfigs.sparkle.footDownFadeRate;
    _sparkle_flash();
}

void Leg::sparkle_footup()
{
    if (DEBUG)
    {
        Serial.print(name); Serial.println(" -> up");
        Serial.print("fade rate to -> "); Serial.println(LWConfigs.sparkle.footUpFadeRate);
    }

    _sparkle_fade_rate = LWConfigs.sparkle.footUpFadeRate;
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
            if (currentTime <= (_lightModeChangeTime + LWConfigs.sparkle.sparkleLength))
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
    if (false)
    {
        Serial.println("_sparkle_flashing " + name);
        Serial.print("    PIXELS_PER_LEG:"); Serial.println(PIXELS_PER_LEG);
        Serial.print("    lower foot border:"); Serial.println(lower_foot_border);
        Serial.print("    upper foot border:"); Serial.println(upper_foot_border);
    }

    for (int i = 0; i < PIXELS_PER_LEG; i++)
    {
        if ((i < lower_foot_border) || (i > upper_foot_border))
            _setPixel(i, COLORS[BLACK], 0x00);
        else
            _setPixel(i, LWConfigs.sparkle.footFlashColor, 0x00);
    }
    _setLightMode(Flash);
}

void Leg::_sparkle_sparkle()
{
    //Serial.println("sparkling " + name);
    float brightness;
    int distance;

    for (int i = 0; i < lower_foot_border; i++)
    {
        distance = lower_foot_border - i;
//         level = byte(random(0, ((LWConfigs.main.maxBrightness/(lower_foot_border * 3)) * (i + 1))));
//         _pixels[i].r = level;
//         _pixels[i].g = level;
//         _pixels[i].b = level;
        int limit = (LWConfigs.main.maxBrightness/(lower_foot_border * 3)) * (i + 1);
        brightness = ((float)map(random(0, limit), 0, limit, 0, 100)) / 100;
        _pixels[i].r = byte((float)LWConfigs.sparkle.legSparkleColor.r * brightness);
        _pixels[i].g = byte((float)LWConfigs.sparkle.legSparkleColor.g * brightness);
        _pixels[i].b = byte((float)LWConfigs.sparkle.legSparkleColor.b * brightness);
    }

    for (int i = upper_foot_border + 1; i < PIXELS_PER_LEG; i++)
    {
        // <gerstle> white sparkle
//         level = byte(random(0, ((LWConfigs.main.maxBrightness/(lower_foot_border * 3)) * (lower_foot_border - (i - upper_foot_border) + 1))));
//         _pixels[i].r = level;
//         _pixels[i].g = level;
//         _pixels[i].b = level;
        int limit = (LWConfigs.main.maxBrightness/(lower_foot_border * 3)) * (lower_foot_border - (i - upper_foot_border) + 1);
        brightness = ((float)map(random(0, limit), 0, limit, 0, 100)) / 100;
        _pixels[i].r = byte((float)LWConfigs.sparkle.legSparkleColor.r * brightness);
        _pixels[i].g = byte((float)LWConfigs.sparkle.legSparkleColor.g * brightness);
        _pixels[i].b = byte((float)LWConfigs.sparkle.legSparkleColor.b * brightness);

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

        for (int i = 0; i < PIXELS_PER_LEG; i++)
            if (((_pixels[i].r - _sparkle_fade_rate) > 0x00) ||
                ((_pixels[i].g - _sparkle_fade_rate) > 0x00) ||
                ((_pixels[i].b - _sparkle_fade_rate) > 0x00))
            {
                
//                 Serial.print("_sparklefade "); Serial.print(name); Serial.print(" to: "); Serial.print(_pixels[i].r - _sparkle_fade_rate); Serial.print(" - "); Serial.print(_pixels[i].g - _sparkle_fade_rate); Serial.print(" - "); Serial.println(_pixels[i].b - _sparkle_fade_rate);
                _setPixel(i, _pixels[i], _sparkle_fade_rate);
                still_fading = true;
            }
            else
                _setPixel(i, COLORS[BLACK], 0x00);

            if (still_fading)
                _setLightMode(Fade);
            else
                off();
    }
    else
        off();
}

void Leg::pulse_pulse(unsigned long currentTime, unsigned long syncTime, int syncLength, int syncValue, bool changeColor)
{
    //unsigned long currentTime = millis();

    if (LWConfigs.pulse.syncLegs)
    {
        //currentTime = syncCurrentTime;
        _lightModeChangeTime = syncTime;
        _pulse_length = syncLength;
    }
    else
        changeColor = false;

    if (currentTime >= (_lightModeChangeTime + (_pulse_length * 2)))
    {
        if (!LWConfigs.pulse.syncLegs)
        {
            _pulse_length = random(LWConfigs.pulse.minPulseTime, LWConfigs.pulse.maxPulseTime);
            _lightModeChangeTime = currentTime;
        }

        _pulse_isDimming = false;
        changeColor = true;
    }
    else if (currentTime >= (_lightModeChangeTime + _pulse_length))
        _pulse_isDimming = true;

    if (changeColor)
    {
        if (LWConfigs.pulse.randomColor)
        {
            int color = random(WHITE, COLOR_COUNT);
            _pulse_color.r = COLORS[color].r;
            _pulse_color.g = COLORS[color].g;
            _pulse_color.b = COLORS[color].b;
        }
        else
        {
            _pulse_color.r = LWConfigs.pulse.color.r;
            _pulse_color.g = LWConfigs.pulse.color.g;
            _pulse_color.b = LWConfigs.pulse.color.b;
        }
    }

    int value = 0;

    if (LWConfigs.pulse.syncLegs)
        value = syncValue;
    else
        if (_pulse_isDimming)
            value = (_pulse_length * 2) - (currentTime - _lightModeChangeTime);
        else
            value = currentTime - _lightModeChangeTime;

    float brightness = ((float)map(value, 0, _pulse_length, 0, 100) / 100);
    
    byte r = byte((float)_pulse_color.r * (brightness * brightness * brightness));
    byte g = byte((float)_pulse_color.g * (brightness * brightness * brightness));
    byte b = byte((float)_pulse_color.b * (brightness * brightness * brightness));

    //Serial.print(newColor.r); Serial.print(" - "); Serial.print(newColor.g); Serial.print(" - "); Serial.println(newColor.b);

    for (int i = 0; i < PIXELS_PER_LEG; i++)
    {
        _pixels[i].r = r;
        _pixels[i].b = b;
        _pixels[i].g = g;
        _setPixel(i, _pixels[i], 0);
    }

}

void Leg::_displayPixels()
{
    for (int i = 0; i < PIXELS_PER_LEG; i++)
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

    _pixels[i].r = map(_pixels[i].r, 0, 255, 0, LWConfigs.main.maxBrightness);
    _pixels[i].g = map(_pixels[i].g, 0, 255, 0, LWConfigs.main.maxBrightness);
    _pixels[i].b = map(_pixels[i].b, 0, 255, 0, LWConfigs.main.maxBrightness);

    if ((i == 0) && false)
    {
        Serial.print("_setPixel:: "); Serial.print(_pixels[i].r); Serial.print(" - "); Serial.print(_pixels[i].g); Serial.print(" - "); Serial.print(_pixels[i].b); Serial.print(" dim: "); Serial.println(dimmer);
    }

    LWUtils.sendColor(_pixels[i]);
}

void Leg::off()
{
    for (int i = 0; i < PIXELS_PER_LEG; i++)
        _setPixel(i, COLORS[BLACK], 0x00);
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
        case MasterOff:
            off();
        case Pulse:
            _pulse_isDimming = false;
            _pulse_length = random(LWConfigs.pulse.minPulseTime, LWConfigs.pulse.maxPulseTime);

            break;
    }
}
