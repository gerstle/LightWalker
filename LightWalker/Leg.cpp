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

void Leg::Init(LWConfigs *c, char n[], int i2c_channel, WalkingModeEnum mode, ADXL345 *adxl, byte half)
{
    int x, y, z;

    config = c;
    strcpy(name, n);
    channel = i2c_channel;
    status = Initialized;
    step = false;
    _lightMode = None;
    _lightModeChangeTime = millis();
    _half = half;

    _sparkle_fade_rate = config->sparkle.footDownFadeRate;

    sparkle_fade_on = true;

    if ((PIXELS_PER_LEG % 2) > 0)
        lower_foot_border = _half - 2;
    else
        lower_foot_border = _half - 3;
    upper_foot_border = _half + 2;

    LWUtils.selectI2CChannels(channel);
    LWUtils.initADXL(adxl);

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

    xStepDuration = 250; // <cgerstle> a step lasts at least this long... ie, two steps can't occur within this time period
    yStepDuration = 750;
    zStepDuration = 750;
    xAvgDiffThreshold = 4;
    yAvgDiffThreshold = 3;
    zAvgDiffThreshold = 3;

    xSignificantlyLowerThanAverageThreshold = 37; 
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

    // <gerstle> gravity
    _indexOne = 0;
    _indexTwo = 1;
    _indexThree = 2;
    _lastXSwitch = millis();
    _lastYSwitch = millis();
}

void Leg::sparkle_footdown()
{
    status = Down;
    _sparkle_fade_rate = config->sparkle.footDownFadeRate;
    _sparkle_flash();
}

void Leg::sparkle_sameStatus()
{
//     Serial.print("leg "); Serial.print(name); Serial.println(" -> same");
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
            //off();
            _sparkle_shimmer();
            break;
    }
}

void Leg::_sparkle_flash()
{
    //Serial.print("flashing "); Serial.println(name);
    for (int i = 0; i < PIXELS_PER_LEG; i++)
    {
        if ((i < lower_foot_border) || (i > upper_foot_border))
            _setPixel(i, COLORS[BLACK], 0x00);
        else
            _setPixel(i, config->sparkle.footFlashColor, 0x00);
    }
    _setLightMode(Flash);
}

void Leg::_sparkle_shimmer()
{
    if (config->main.minBrightness == 0)
    {
        off();
        return;
    }

    float brightness;

    // leading leg
    for (int i = 0; i < PIXELS_PER_LEG; i++)
    {
        if ((i <= (_half + 1)) && (i >= (_half - 1)))
        {
            brightness = ((float)config->main.maxBrightness / 20) / 100;  // <cgerstle> somewheres between 0 and 5% of max
            _pixels[i].r = byte((float)config->sparkle.footSparkleColor.r * brightness);
            _pixels[i].g = byte((float)config->sparkle.footSparkleColor.g * brightness);
            _pixels[i].b = byte((float)config->sparkle.footSparkleColor.b * brightness);
        }
        else if (random(0, 50) == 0)
        {
            brightness = ((float)random(0, config->main.maxBrightness / 20)) / 100;  // <cgerstle> somewheres between 0 and 5% of max
            _pixels[i].r = byte((float)config->sparkle.legSparkleColor.r * brightness);
            _pixels[i].g = byte((float)config->sparkle.legSparkleColor.g * brightness);
            _pixels[i].b = byte((float)config->sparkle.legSparkleColor.b * brightness);
        }
        LWUtils.sendColor(_pixels[i]);
    }

    _setLightMode(Off);
}

void Leg::_sparkle_sparkle()
{
    //Serial.print("\tsparkling "); Serial.println(name);
    float brightness;
    int distance;

    // leading leg
    for (int i = 0; i < lower_foot_border; i++)
    {
        distance = lower_foot_border - i;
        int limit = (config->main.maxBrightness/(lower_foot_border * 3)) * (i + 1);
        brightness = ((float)map(random(0, limit), 0, limit, 0, 100)) / 100;
        _pixels[i].r = byte((float)config->sparkle.legSparkleColor.r * brightness);
        _pixels[i].g = byte((float)config->sparkle.legSparkleColor.g * brightness);
        _pixels[i].b = byte((float)config->sparkle.legSparkleColor.b * brightness);
    }

    //leg
    if (currentTime > (_lightModeChangeTime + config->sparkle.flashLength))
        for (int i = lower_foot_border; i <= upper_foot_border; i++)
        {
            _pixels[i].r = config->sparkle.footSparkleColor.r;
            _pixels[i].g = config->sparkle.footSparkleColor.g;
            _pixels[i].b = config->sparkle.footSparkleColor.b;
        }

    // trailing leg
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
    //Serial.print("\t\tfading "); Serial.println(name);
    if (sparkle_fade_on)
    {
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
                _sparkle_shimmer();
    }
    else
        _sparkle_shimmer();
}

void Leg::pulse_pulse(unsigned long syncTime, int syncLength, int syncValue, bool changeColor)
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
    brightness = brightness * brightness * brightness;

    byte min = 0;
    bool setMin = false;

    byte r = byte((float)_pulse_color.r * brightness);
    //byte g = map(byte((float)_pulse_color.g * brightness), 0, 255, min, 255);
    byte g = byte((float)_pulse_color.g * brightness);
    byte b = byte((float)_pulse_color.b * brightness);

    if ((r < config->main.minBrightness) && 
        (g < config->main.minBrightness) &&
        (b < config->main.minBrightness))
        setMin = true;

    if (setMin && (_pulse_color.r > 0))
        r = map(r, 0, 255, config->main.minBrightness, 255);
    if (setMin && (_pulse_color.g > 0))
        g = map(g, 0, 255, config->main.minBrightness, 255);
    if (setMin && (_pulse_color.b > 0))
        b = map(b, 0, 255, config->main.minBrightness, 255);

    for (int i = 0; i < PIXELS_PER_LEG; i++)
    {
        RGB newColor;

        // <gerstle> don't change from non-zero to zero!
        if ((_pixels[i].r != 0) && (r == 0))
            newColor.r = _pixels[i].r;
        else
            newColor.r = r;

        if ((_pixels[i].g != 0) && (g == 0))
            newColor.g = _pixels[i].g;
        else
            newColor.g = g;

        if ((_pixels[i].b != 0) && (b == 0))
            newColor.b = _pixels[i].b;
        else
            newColor.b = b;
    
        _setPixel(i, newColor, 0);
    }

//     if (channel == 1)
//     {
//         Serial.print("\t\t"); LWUtils.printRGB(_pixels[0], true);
//     }
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
        int lower_threshold = _half - (level_percent * _half);
        int upper_threshold = (level_percent * _half) + _half;

        for (i = 0; i < PIXELS_PER_LEG; i++)
        {
            if (((i < _half) && (i < lower_threshold)) ||
                ((i > _half) && (i > upper_threshold)))
            {
                if (config->main.minBrightness > 0)
                {
                    _pixels[i].r = config->equalizer.minColor.r;
                    _pixels[i].g = config->equalizer.minColor.g;
                    _pixels[i].b = config->equalizer.minColor.b;
                }
                else
                {
                    _pixels[i].r = 0;
                    _pixels[i].g = 0;
                    _pixels[i].b = 0;
                }
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
    if (color.r > dimmer)
        color.r = color.r - dimmer;
    else
        color.r = 0;

    if (color.g > dimmer)
        color.g = color.g - dimmer;
    else
        color.g = 0;

    if (color.b > dimmer)
        color.b = color.b - dimmer;
    else
        color.b = 0;

    if ((channel == 1) && (i == 0) && false)
    {
        Serial.print("_setPixel:: "); Serial.print(color.r); Serial.print(" - "); Serial.print(color.g); Serial.print(" - "); Serial.print(color.b); Serial.print(" dim: "); Serial.println(dimmer);
    }

    if (color.r > 0)
        _pixels[i].r = map(color.r, config->main.minBrightness, 255, config->main.minBrightness, config->main.maxBrightness);
    else
        _pixels[i].r = 0;

    if (color.g > 0)
        _pixels[i].g = map(color.g, config->main.minBrightness, 255, config->main.minBrightness, config->main.maxBrightness);
    else
        _pixels[i].g = 0;

    if (color.b > 0)
        _pixels[i].b = map(color.b, config->main.minBrightness, 255, config->main.minBrightness, config->main.maxBrightness);
    else
        _pixels[i].b = 0;

    if ((channel == 1) && (i == 0) && false)
    {
        Serial.print("\t\t-> "); Serial.print(_pixels[i].r); Serial.print(" - "); Serial.print(_pixels[i].g); Serial.print(" - "); Serial.print(_pixels[i].b); Serial.print(" dim: "); Serial.println(dimmer);
    }

    LWUtils.sendColor(_pixels[i]);
}

// <cgerstle> takes in an rgb and checks that it's
// above the minimum. If it's not and some have value, scale up to min
// if all zero... scale default to min
void Leg::upToMin(byte *r, byte *g, byte *b, RGB defaultColor)
{
    if (channel == 1)
    {
        Serial.print("got "); LWUtils.printRGB(*r, *g, *b, false);
    }

    if ((*r < config->main.minBrightness) && 
        (*g < config->main.minBrightness) &&
        (*b < config->main.minBrightness))
    {
        byte _r, _g, _b;

        if ((*r > 0) ||
            (*g > 0) ||
            (*b > 0))
        {
            _r = *r;
            _g = *g;
            _b = *b;
        }
        else
        {
            _r = defaultColor.r;
            _g = defaultColor.g;
            _b = defaultColor.b;
        }
        
        // <cgerstle> scale min
        int minimum = 255;
        if (defaultColor.r > 0)
            minimum = min(minimum, defaultColor.r);
        if (defaultColor.g > 0)
            minimum = min(minimum, defaultColor.g);
        if (defaultColor.b > 0)
            minimum = min(minimum, defaultColor.b);

        float scale = ((float)config->main.minBrightness)/((float)minimum);
        *r = (byte)(scale * (float)defaultColor.r);
        *g = (byte)(scale * (float)defaultColor.g);
        *b = (byte)(scale * (float)defaultColor.b);

        if (channel == 1)
        {
            Serial.print("scale -> "); LWUtils.printRGB(*r, *g, *b, false);
        }
    }
    else if (channel == 1)
        Serial.print(".");
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

void Leg::setWalkingMode(WalkingModeEnum mode, ADXL345 *adxl)
{
    off();
    currentTime = _lightModeChangeTime = millis();

    // <gerstle> sparkle stuff
    _walkingMode = mode;
    _lightMode = Off;

    switch (_walkingMode)
    {
        case masterOff:
            off();
        case pulse:
            _pulse_color.r = config->pulse.color.r;
            _pulse_color.g = config->pulse.color.g;
            _pulse_color.b = config->pulse.color.b;
            _pulse_isDimming = false;
            _pulse_length = random(config->pulse.minPulseTime, config->pulse.maxPulseTime);
            break;
    }
}

int x, y, z;
void Leg::detectStep(ADXL345 *adxl)
{
    bool stepDetected = false;

    LWUtils.selectI2CChannels(channel);
    adxl->readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

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

    if (x < (xAverage - xSignificantlyLowerThanAverageThreshold))
        readyForStep = true;

//     Serial.print(x); Serial.print("\t"); Serial.print(y); Serial.print("\t"); Serial.print(z);
//     Serial.print("\t"); Serial.print(xAverage); Serial.print("\t"); Serial.print(xAverageOld);
//     Serial.print("\t"); Serial.print(yAverage); Serial.print("\t"); Serial.print(yAverageOld);
//     Serial.print("\t"); Serial.print(zAverage); Serial.print("\t"); Serial.print(zAverageOld);
//     Serial.print("\t"); Serial.print(readyForStep);

    if (readyForStep && (currentTime > (lastSharpPeakTime + xStepDuration)))
    {
        step = false;
        if ((x > xAverage) && (xAverage >= (xAverageOld + xAvgDiffThreshold)))
        {
            stepDetected = true;
            readyForStep = false;
        }
    }


    if (readyForStep && (currentTime > (lastSharpPeakTime + zStepDuration)))
        if (!stepDetected && (zAverage >= (zAverageOld + zAvgDiffThreshold)))
            stepDetected = true;

    if (readyForStep && (currentTime > (lastSharpPeakTime + yStepDuration)))
        if (!stepDetected && (yAverage >= (yAverageOld + yAvgDiffThreshold)))
            stepDetected = true;

    if (stepDetected)
    {
        Serial.print("\t\t"); Serial.print(name); Serial.println("\tSTEP!");
        lastSharpPeakTime = millis();
        step = true;
        sparkle_footdown();
        sparkle_fade_on = true;
    }
    else
    {
        sparkle_sameStatus();
//         Serial.println();
    }

    xAverageOld = xAverage;
    yAverageOld = yAverage;
    zAverageOld = zAverage;

    valueIndex++;
}

void Leg::gravity2Lights(ADXL345 *adxl)
{
    bool print = false;
	double xyz[3];

    LWUtils.selectI2CChannels(channel);
    adxl->getAccelemeter(xyz);

    currentTime = millis();

    if (print)
    {
        Serial.print(xyz[0]); Serial.print(","); Serial.print(xyz[1]); Serial.print(","); Serial.print(xyz[2]);
    }

    if (_indexOne == 2)
    {
        if (xyz[_indexOne] > 0)
            _pixels[0].r = map(abs(xyz[_indexOne]) * 100, 0, 192, 0, 255);
        else
            _pixels[0].r = 0x0;
    }
    else
        _pixels[0].r = map(abs(xyz[_indexOne]) * 100, 0, 192, 0, 255);

    if (_indexTwo == 2)
    {
        if (xyz[_indexTwo] > 0)
            _pixels[0].g = map(abs(xyz[_indexTwo]) * 100, 0, 192, 0, 255);
        else
            _pixels[0].g = 0x0;
    }
    else
        _pixels[0].g = map(abs(xyz[_indexTwo]) * 100, 0, 192, 0, 255);

    if (_indexThree == 2)
    {
        if (xyz[_indexThree] > 0)
            _pixels[0].b = map(abs(xyz[_indexThree]) * 100, 0, 192, 0, 255);
        else
            _pixels[0].b = 0x0;
        if (print)
        {
            Serial.print("\tb: "); Serial.print(_pixels[0].b);
        }
    }
    else
        _pixels[0].b = map(abs(xyz[_indexThree]) * 100, 0, 192, 0, 255);


    if (config->gravity.rotate)
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
            _lastXSwitch = millis();
        }

        if ((abs(xyz[1]) > 0.97) && _canYSwitch)
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
            _lastYSwitch = millis();
        }

        if ((abs(xyz[0]) < 0.3) && (currentTime > (_lastXSwitch + 600)))
            _canXSwitch = true;

        if ((abs(xyz[1]) < 0.3) && (currentTime > (_lastYSwitch + 600)))
            _canYSwitch = true;
    }

    LWUtils.sendColor(_pixels[0]);
    for (int i = 1; i < PIXELS_PER_LEG; i++)
    {
        _pixels[i].r = _pixels[0].r;
        _pixels[i].g = _pixels[0].g;
        _pixels[i].b = _pixels[0].b;
        LWUtils.sendColor(_pixels[i]);
    }
    if (print)
        Serial.println("");
}
