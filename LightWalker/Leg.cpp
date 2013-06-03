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
    name[0] = '\0';
}

void Leg::Init(LWConfigs *c, char n[], int i2c_channel, WalkingModeEnum mode, ADXL345 *adxl)
{
    int x, y, z, half;

    config = c;
    strcpy(name, n);
    channel = i2c_channel;
    status = Initialized;
    step = false;
    _lightMode = None;
    _lightModeChangeTime = millis();

    _sparkle_fade_rate = config->sparkle.footDownFadeRate;

    sparkle_fade_on = true;

    half = PIXELS_PER_LEG / 2;

    if ((PIXELS_PER_LEG % 2) > 0)
        lower_foot_border = half - 2;
    else
        lower_foot_border = half - 3;
    upper_foot_border = half + 2;

    LWUtils.selectI2CChannels(channel);
    LWUtils.initADXL(*adxl);

    valueIndex = 0;

    xTotal = 0;
    xAverage = 0;
    xAverageOld = 0;

    yTotal = 0;
    yAverage = 0;
    yAverageOld = 0;

    zTotal = 0;
    zAverage = 0;
    zAverageOld = 0;

    xStepDuration = 750; // <cgerstle> a step lasts at least this long... ie, two steps can't occur within this time period
    yStepDuration = 750;
    zStepDuration = 250;
    xAvgDiffThreshold = 3;
    yAvgDiffThreshold = 3;
    zAvgDiffThreshold = 3;

    zSignificantlyLowerThanAverageThreshold = 35; 
    readyForStep = true;

    for (valueIndex = 0; valueIndex < ADXL_VALUE_COUNT; valueIndex++)
    {
        adxl->readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

        x = abs(x);
        y = abs(y);
        z = z;

        xValues[valueIndex] = x;
        yValues[valueIndex] = y;
        zValues[valueIndex] = z;

        xTotal += xValues[valueIndex];
        yTotal += yValues[valueIndex];
        zTotal += zValues[valueIndex];
    }

    xAverage = xTotal / ADXL_VALUE_COUNT;
    yAverage = yTotal / ADXL_VALUE_COUNT;
    zAverage = zTotal / ADXL_VALUE_COUNT;
    lastSharpPeakTime = millis();
}

void Leg::sparkle_footdown()
{
    status = Down;
    if (DEBUG)
    {
        Serial.print(name); Serial.println(" -> down");
    }

    _sparkle_fade_rate = config->sparkle.footDownFadeRate;
    _sparkle_flash();
}

void Leg::sparkle_footup()
{
    if (DEBUG)
    {
        Serial.print(name); Serial.println(" -> up");
    }

    _sparkle_fade_rate = config->sparkle.footUpFadeRate;
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
            if (currentTime <= (_lightModeChangeTime + config->sparkle.sparkleLength))
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
    //Serial.println("flashing " + name);
    for (int i = 0; i < PIXELS_PER_LEG; i++)
    {
        if ((i < lower_foot_border) || (i > upper_foot_border))
            _setPixel(i, COLORS[BLACK], 0x00);
        else
            _setPixel(i, config->sparkle.footFlashColor, 0x00);
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
        int limit = (config->main.maxBrightness/(lower_foot_border * 3)) * (i + 1);
        brightness = ((float)map(random(0, limit), 0, limit, 0, 100)) / 100;
        _pixels[i].r = byte((float)config->sparkle.legSparkleColor.r * brightness);
        _pixels[i].g = byte((float)config->sparkle.legSparkleColor.g * brightness);
        _pixels[i].b = byte((float)config->sparkle.legSparkleColor.b * brightness);
    }

    for (int i = upper_foot_border + 1; i < PIXELS_PER_LEG; i++)
    {
        // <gerstle> white sparkle
        int limit = (config->main.maxBrightness/(lower_foot_border * 3)) * (lower_foot_border - (i - upper_foot_border) + 1);
        brightness = ((float)map(random(0, limit), 0, limit, 0, 100)) / 100;
        _pixels[i].r = byte((float)config->sparkle.legSparkleColor.r * brightness);
        _pixels[i].g = byte((float)config->sparkle.legSparkleColor.g * brightness);
        _pixels[i].b = byte((float)config->sparkle.legSparkleColor.b * brightness);
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
    if (config->pulse.syncLegs)
    {
        _lightModeChangeTime = syncTime;
        _pulse_length = syncLength;
    }
    else
        changeColor = false;

    if (currentTime >= (_lightModeChangeTime + (_pulse_length * 2)))
    {
        if (!config->pulse.syncLegs)
        {
            _pulse_length = random(config->pulse.minPulseTime, config->pulse.maxPulseTime);
            _lightModeChangeTime = currentTime;
        }

        _pulse_isDimming = false;
        changeColor = true;
    }
    else if (currentTime >= (_lightModeChangeTime + _pulse_length))
        _pulse_isDimming = true;

    if (changeColor)
    {
        if (config->pulse.randomColor)
        {
            int color = random(WHITE, COLOR_COUNT);
            _pulse_color.r = COLORS[color].r;
            _pulse_color.g = COLORS[color].g;
            _pulse_color.b = COLORS[color].b;
        }
        else
        {
//         Serial.print("color to:: "); Serial.print(config->pulse.color.r); Serial.print(" - "); Serial.print(config->pulse.color.g); Serial.print(" - "); Serial.println(config->pulse.color.b);
            _pulse_color.r = config->pulse.color.r;
            _pulse_color.g = config->pulse.color.g;
            _pulse_color.b = config->pulse.color.b;
        }
    }

    int value = 0;

    if (config->pulse.syncLegs)
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

    for (int i = 0; i < PIXELS_PER_LEG; i++)
    {
        _pixels[i].r = r;
        _pixels[i].b = b;
        _pixels[i].g = g;
        _setPixel(i, _pixels[i], 0);
    }

}

void Leg::equalizer_listen(float level_percent, byte r, byte g, byte b)
{
    if (config->equalizer.allLights)
    {
        for (int i = 0; i < PIXELS_PER_LEG; i++)
        {
            _pixels[i].r = r;
            _pixels[i].b = b;
            _pixels[i].g = g;
            _setPixel(i, _pixels[i], 0);
        }
    }
    else
    {
        int i = 0;
        int half = PIXELS_PER_LEG / 2;
        int lower_threshold = half - (level_percent * half);
        int upper_threshold = (level_percent * half) + half;

        for (i = 0; i < PIXELS_PER_LEG; i++)
        {
            if (((i < half) && (i < lower_threshold)) ||
                ((i > half) && (i > upper_threshold)))
            {
                _pixels[i].r = 0;
                _pixels[i].g = 0;
                _pixels[i].b = 0;
            }
            else
            {
                _pixels[i].r = config->equalizer.color.r;
                _pixels[i].g = config->equalizer.color.g;
                _pixels[i].b = config->equalizer.color.b;
            }

            _setPixel(i, _pixels[i], 0);
        }
    }
}

void Leg::_displayPixels()
{
    for (int i = 0; i < PIXELS_PER_LEG; i++)
        LWUtils.sendColor(_pixels[i]);
}

void Leg::_setPixel(int i, RGB color, byte dimmer)
{
//     if ((i == 0) && false)
//     {
//         Serial.print("_setPixel before:: "); Serial.print(color.r); Serial.print(" - "); Serial.print(color.g); Serial.print(" - "); Serial.print(color.b); Serial.print(" dim: "); Serial.println(dimmer);
//     }

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

    _pixels[i].r = map(_pixels[i].r, 0, 255, 0, config->main.maxBrightness);
    _pixels[i].g = map(_pixels[i].g, 0, 255, 0, config->main.maxBrightness);
    _pixels[i].b = map(_pixels[i].b, 0, 255, 0, config->main.maxBrightness);

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
    off();
    _lightModeChangeTime = millis();

    // <gerstle> sparkle stuff
    _walkingMode = mode;
    _lightMode = Off;

    switch (_walkingMode)
    {
        case masterOff:
            off();
        case pulse:
            _pulse_isDimming = false;
            _pulse_length = random(config->pulse.minPulseTime, config->pulse.maxPulseTime);
            break;
        case equalizer:
            // <gerstle> do nothing
            break;
        case gravity:
            // <gerstle> do nothing
            break;
    }
}

int x, y, z;
void Leg::detectStep(ADXL345 *adxl)
{
    unsigned long currentTime = millis();
    bool stepDetected = false;

    LWUtils.selectI2CChannels(channel);
    adxl->readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
    Serial.println(x);

    x = abs(x);
    y = abs(y);
    z = z;

    if (valueIndex >= ADXL_VALUE_COUNT)
        valueIndex = 0;

    xTotal = xTotal - xValues[valueIndex] + x;
    xValues[valueIndex] = x;
    xAverage = xTotal / ADXL_VALUE_COUNT;

    yTotal = yTotal - yValues[valueIndex] + y;
    yValues[valueIndex] = y;
    yAverage = yTotal / ADXL_VALUE_COUNT;
   
    zTotal = zTotal - zValues[valueIndex] + z;
    zValues[valueIndex] = z;
    zAverage = zTotal / ADXL_VALUE_COUNT;

    if (z < (zAverage - zSignificantlyLowerThanAverageThreshold))
        readyForStep = true;

//     Serial.print(x); Serial.print("\t"); Serial.print(y); Serial.print("\t"); Serial.print(z);
//         Serial.print("\t"); Serial.print(xAverage); Serial.print("\t"); Serial.print(xAverageOld);
//         Serial.print("\t"); Serial.print(yAverage); Serial.print("\t"); Serial.print(yAverageOld);
//         Serial.print("\t"); Serial.print(zAverage); Serial.print("\t"); Serial.print(zAverageOld);
//         Serial.print("\t"); Serial.print(readyForStep);

    if (readyForStep && (currentTime > (lastSharpPeakTime + zStepDuration)))
        if ((z > zAverage) && (zAverage >= (zAverageOld + zAvgDiffThreshold)))
        {
            stepDetected = true;
            readyForStep = false;
        }


    if (readyForStep && (currentTime > (lastSharpPeakTime + xStepDuration)))
        if (!stepDetected && (xAverage >= (xAverageOld + xAvgDiffThreshold)))
            stepDetected = true;

    if (readyForStep && (currentTime > (lastSharpPeakTime + yStepDuration)))
        if (!stepDetected && (yAverage >= (yAverageOld + yAvgDiffThreshold)))
            stepDetected = true;

    if (stepDetected)
    {
        //Serial.println("\t\tSTEP!");
        lastSharpPeakTime = millis();
    }

    xAverageOld = xAverage;
    yAverageOld = yAverage;
    zAverageOld = zAverage;

    valueIndex++;
}
