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

void Leg::init(LWConfigs *c, char *n, int i2c_channel, WalkingModeEnum mode, ADXL345 *adxl, byte count, byte half, RGB *p)
{
    config = c;
    strcpy(name, n);
    channel = i2c_channel;
    status = Initialized;
    step = false;
    _lightMode = None;
    _lightModeChangeTime = millis();
    _pixels = p;
    setPixelCount(count, half);

    LWUtils.selectI2CChannels(channel);
    LWUtils.initADXL(adxl);

    // <gerstle> init ADXL EMA's
    int x, y, z;
    xStepDuration = 250; // <cgerstle> a step lasts at least this long... ie, two steps can't occur within this time period
    yStepDuration = 750;
    zStepDuration = 750;
    xAvgDiffThreshold = 160;
    yAvgDiffThreshold = 80;
    zAvgDiffThreshold = 80;
    xSignificantlyLowerThanAverageThreshold = 45; 
    readyForStep = false;
    lastSharpPeakTime = millis();
    /*
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
    */
    int valueIndex = 0;
    int xValueTotal, yValueTotal, zValueTotal  = 0;
    for (valueIndex = 0; valueIndex < ADXL_VALUE_COUNT; valueIndex++)
    {
        adxl->readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

        x = x;
        y = abs(y);
        z = abs(z);

        xValueTotal += x;
        yValueTotal += y;
        zValueTotal += z;

        xNMinus2 = xNMinus1;
        xNMinus1 = x;

        yNMinus2 = yNMinus1;
        yNMinus1 = y;

        zNMinus2 = zNMinus1;
        zNMinus1 = z;
    }

    xEMA = xValueTotal / ADXL_VALUE_COUNT;
    yEMA = yValueTotal / ADXL_VALUE_COUNT;
    zEMA = zValueTotal / ADXL_VALUE_COUNT;

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
    _sparkle_flash();
}

void Leg::sparkle_sameStatus()
{
    // Serial.print("leg "); Serial.print(name); Serial.println(" -> same");
    switch (_lightMode)
    {
        case Flash:
                _sparkle_sparkle();
            break;

        case SparkleSparkle:
                _sparkle_sparkle();
            break;

        case Fade:
            _sparkle_fade();
            break;

        case Off:
            _sparkle_shimmer();
            break;
    }
}

void Leg::_sparkle_flash()
{
    // Serial.print("flashing "); Serial.println(name);
    for (int i = 0; i < _pixelCount; i++)
    {
        if ((i < _lower_foot_border) || (i > _upper_foot_border))
            _setPixel(i, COLORS[BLACK], 0);
        else
            _setPixel(i, config->sparkle.footFlashColor, 0);
    }
    _setLightMode(Flash);
}

void Leg::_sparkle_shimmer()
{
    // Serial.print("shimmering "); Serial.println(name);
    float max = ((float) min(40, config->main.maxBrightness)) / 100;
    float brightness = 0;
    int dice = 0;

    // leading leg
    for (int i = 0; i < _pixelCount; i++)
    {
        dice = random(0, 100);
        if ((i <= (_half + 1)) && (i >= (_half - 1)))
        {
            _pixels[i].r = byte((float)config->sparkle.sparkleColor.r * max);
            _pixels[i].g = byte((float)config->sparkle.sparkleColor.g * max);
            _pixels[i].b = byte((float)config->sparkle.sparkleColor.b * max);
        }
        else if ((config->main.minBrightness > 0) && (dice == 0))
        {
            brightness = ((float) random(0, 20)) / 100 * max;
            _pixels[i].r = byte((float)config->sparkle.sparkleColor.r * brightness);
            _pixels[i].g = byte((float)config->sparkle.sparkleColor.g * brightness);
            _pixels[i].b = byte((float)config->sparkle.sparkleColor.b * brightness);
        }
        else if (config->main.minBrightness == 0)
        {
            _pixels[i].r = 0;
            _pixels[i].g = 0;
            _pixels[i].b = 0;
        }
        _setPixel(i, _pixels[i], 0);
    }

    _setLightMode(Off);
}

void Leg::_sparkle_sparkle()
{
    //Serial.print("\tsparkling "); Serial.println(name);
    float brightness;
    float rand;

    // leading leg
    for (int i = 0; i < _lower_foot_border; i++)
    {
        brightness = ((float) i / (float) _lower_foot_border);
        rand = (float)random(0, min(20, brightness * 100)) / 100;
        brightness = brightness - rand;
        //Serial.print(i); Serial.print("\t"); Serial.print(brightness); Serial.print("\t"); Serial.println(rand);
        _pixels[i].r = byte((float)config->sparkle.sparkleColor.r * brightness);
        _pixels[i].g = byte((float)config->sparkle.sparkleColor.g * brightness);
        _pixels[i].b = byte((float)config->sparkle.sparkleColor.b * brightness);
        _setPixel(i, _pixels[i], 0);
    }

    //foot
    RGB *footColor = &config->sparkle.footFlashColor;
    if (currentTime > (_lightModeChangeTime + config->sparkle.flashLength))
        footColor = &config->sparkle.sparkleColor;

    for (int i = _lower_foot_border; i <= _upper_foot_border; i++)
    {
        _pixels[i].r = (*footColor).r;
        _pixels[i].g = (*footColor).g;
        _pixels[i].b = (*footColor).b;
        _setPixel(i, _pixels[i], 0);
    }

    // trailing leg
    for (int i = _upper_foot_border + 1; i < _pixelCount; i++)
    {
        brightness = ((float) (_pixelCount - 1 - i) / (float) _lower_foot_border);
        rand = (float)random(0, min(20, brightness * 100)) / 100;
        brightness = brightness - rand;
        //Serial.print(i); Serial.print("\t"); Serial.print(brightness); Serial.print("\t"); Serial.println(rand);
        _pixels[i].r = byte((float)config->sparkle.sparkleColor.r * brightness);
        _pixels[i].g = byte((float)config->sparkle.sparkleColor.g * brightness);
        _pixels[i].b = byte((float)config->sparkle.sparkleColor.b * brightness);
        _setPixel(i, _pixels[i], 0);
    }

    if (currentTime <= (_lightModeChangeTime + config->sparkle.sparkleLength))
        _setLightMode(SparkleSparkle);
    else
    {
        for (int i = 0; i < _pixelCount; i++)
            if ((i >= _lower_foot_border) && (i <= _upper_foot_border))
            {
                _pixels[i].r = byte((float)config->sparkle.sparkleColor.r);
                _pixels[i].g = byte((float)config->sparkle.sparkleColor.g);
                _pixels[i].b = byte((float)config->sparkle.sparkleColor.b);
            }
        _setLightMode(Fade);
    }
}

void Leg::_sparkle_fade()
{
    // Serial.print("\t\tfading "); Serial.println(name);
    int still_fading = 0;

    for (int i = 0; i < _pixelCount; i++)
        if (((_pixels[i].r - config->sparkle.fadeRate) > 0) ||
            ((_pixels[i].g - config->sparkle.fadeRate) > 0) ||
            ((_pixels[i].b - config->sparkle.fadeRate) > 0))
        {
            _setPixel(i, _pixels[i], config->sparkle.fadeRate);
            still_fading++;
        }
        else
            _setPixel(i, _pixels[i], 0);

        if ((config->main.minBrightness > 0) && (still_fading > (_pixelCount / 2)))
            _setLightMode(Fade);
        else if ((config->main.minBrightness == 0) && (still_fading))
            _setLightMode(Fade);
        else
            _setLightMode(Off);
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
            int color = random(1, COLOR_COUNT);
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
    byte g = byte((float)_pulse_color.g * brightness);
    byte b = byte((float)_pulse_color.b * brightness);

    r = map(r, 0, 255, config->main.minBrightness, config->main.maxBrightness);
    g = map(g, 0, 255, config->main.minBrightness, config->main.maxBrightness);
    b = map(b, 0, 255, config->main.minBrightness, config->main.maxBrightness);

    for (int i = 0; i < _pixelCount; i++)
    {
        RGB newColor;

        // <gerstle> don't change from non-zero to zero!
        if ((_pixels[i].r != 0) && (r == 0) && (config->main.minBrightness > 0))
            newColor.r = _pixels[i].r;
        else
            newColor.r = r;

        if ((_pixels[i].g != 0) && (g == 0) && (config->main.minBrightness > 0))
            newColor.g = _pixels[i].g;
        else
            newColor.g = g;

        if ((_pixels[i].b != 0) && (b == 0) && (config->main.minBrightness > 0))
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
        for (int i = 0; i < _pixelCount; i++)
        {
            _pixels[i].r = r;
            _pixels[i].g = g;
            _pixels[i].b = b;
            _setPixel(i, _pixels[i], 0);
        }
    }
    else
    {
        int i = 0;
        int lower_threshold = _half - (level_percent * _half);
        int upper_threshold = (level_percent * _half) + _half;
        int rainbow_color_offset = 2;
        int rainbow_color;

        for (i = 0; i < _pixelCount; i++)
        {
            if (((i < _half) && (i < lower_threshold)) ||
                ((i > _half) && (i > upper_threshold)))
            {
                if (config->main.minBrightness > 0)
                {
                    if (config->equalizer.rainbow)
                    {
                        rainbow_color = (i / _pixelsPerColor) + rainbow_color_offset;
                        _pixels[i].r = config->main.minColors[rainbow_color].r;
                        _pixels[i].g = config->main.minColors[rainbow_color].g;
                        _pixels[i].b = config->main.minColors[rainbow_color].b;
                    }
                    else
                    {
                        _pixels[i].r = config->equalizer.minColor.r;
                        _pixels[i].g = config->equalizer.minColor.g;
                        _pixels[i].b = config->equalizer.minColor.b;
                    }
                }
                else
                {
                    _pixels[i].r = 0;
                    _pixels[i].g = 0;
                    _pixels[i].b = 0;
                }
            }
            else if (config->equalizer.rainbow)
            {
                rainbow_color = (i / _pixelsPerColor) + rainbow_color_offset;
                _pixels[i].r = config->main.maxColors[rainbow_color].r;
                _pixels[i].g = config->main.maxColors[rainbow_color].g;
                _pixels[i].b = config->main.maxColors[rainbow_color].b;
            }
            else
            {
                _pixels[i].r = config->equalizer.maxColor.r;
                _pixels[i].g = config->equalizer.maxColor.g;
                _pixels[i].b = config->equalizer.maxColor.b;
            }

            _setPixel(i, _pixels[i], 0);
        }
    }
}

void Leg::_setPixel(int i, RGB color, byte dimmer)
{
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

    LWUtils.sendColor(_pixels[i]);
}

// <cgerstle> takes in an rgb and checks that it's
// above the minimum. If it's not and some have value, scale up to min
// if all zero... scale default to min
void Leg::upToMin(byte *r, byte *g, byte *b, RGB defaultColor)
{
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
    }
}

void Leg::off()
{
    for (int i = 0; i < _pixelCount; i++)
        _setPixel(i, COLORS[BLACK], 0);
    _setLightMode(Off);
}

void Leg::_setLightMode(LightModeEnum mode)
{
    if (_lightMode != mode)
    {
        _lightModeChangeTime = millis();
    }
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
        case equalizer:
            _rainbowColorCount = COLOR_COUNT - 2;
            _pixelsPerColor = _pixelCount / _rainbowColorCount;
            break;
    }
}

int x, y, z;
void Leg::detectStep(ADXL345 *adxl)
{
    bool stepDetected = false;

    LWUtils.selectI2CChannels(channel);
    adxl->readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

    x = x;
    y = abs(y);
    z = abs(z);

    xEMA = (float)(x- xEMA) * ((float)2 / (float)(ADXL_VALUE_COUNT + 1)) + xEMA;
    yEMA = (float)(y- yEMA) * ((float)2 / (float)(ADXL_VALUE_COUNT + 1)) + yEMA;
    zEMA = (float)(z- zEMA) * ((float)2 / (float)(ADXL_VALUE_COUNT + 1)) + zEMA;

    xNMinus2 = xNMinus1;
    xNMinus1 = x;

    yNMinus2 = yNMinus1;
    yNMinus1 = y;

    zNMinus2 = zNMinus1;
    zNMinus1 = z;

/*
    Serial.print(x); Serial.print("\t"); Serial.print(y); Serial.print("\t"); Serial.print(z);
    Serial.print("\t"); Serial.print(xEMA);
    Serial.print("\t"); Serial.print(yEMA);
    Serial.print("\t"); Serial.print(zEMA);
    Serial.print("\t"); Serial.println(readyForStep);
*/

    if (x < (xEMA - xSignificantlyLowerThanAverageThreshold))
        readyForStep = true;

    if (readyForStep && (currentTime > (lastSharpPeakTime + xStepDuration)))
    {
        step = false;
        if (x > (xEMA + xAvgDiffThreshold))
        {
            stepDetected = true;
            readyForStep = false;
            //Serial.print("X\t\t");
        }
    }

    if (readyForStep && (currentTime > (lastSharpPeakTime + yStepDuration)))
        if (!stepDetected && (y >= (yEMA + yAvgDiffThreshold)))
        {
            stepDetected = true;
            //Serial.print("y\t\t");
        }


    if (readyForStep && (currentTime > (lastSharpPeakTime + zStepDuration)))
        if (!stepDetected && (z >= (zEMA + zAvgDiffThreshold)))
        {
            stepDetected = true;
            //Serial.print("Z\t\t");
        }

    if (stepDetected)
    {
        //Serial.print(name); Serial.println("\tSTEP!");
        lastSharpPeakTime = millis();
        step = true;
        sparkle_footdown();
    }
    else
        sparkle_sameStatus();
}

void Leg::gravity2Lights(ADXL345 *adxl)
{
	double xyz[3];

    LWUtils.selectI2CChannels(channel);
    adxl->getAcceleration(xyz);

    currentTime = millis();

    //Serial.print(xyz[0]); Serial.print(","); Serial.print(xyz[1]); Serial.print(","); Serial.print(xyz[2]);

    if (_indexOne == 2)
    {
        if (xyz[_indexOne] > 0)
            _pixels[0].r = map(abs(xyz[_indexOne]) * 100, 0, 192, config->main.minBrightness, config->main.maxBrightness);
        else
            _pixels[0].r = 0;
    }
    else
        _pixels[0].r = map(abs(xyz[_indexOne]) * 100, 0, 192, config->main.minBrightness, config->main.maxBrightness);

    if (_indexTwo == 2)
    {
        if (xyz[_indexTwo] > 0)
            _pixels[0].g = map(abs(xyz[_indexTwo]) * 100, 0, 192, config->main.minBrightness, config->main.maxBrightness);
        else
            _pixels[0].g = 0;
    }
    else
        _pixels[0].g = map(abs(xyz[_indexTwo]) * 100, 0, 192, config->main.minBrightness, config->main.maxBrightness);

    if (_indexThree == 2)
    {
        if (xyz[_indexThree] > 0)
            _pixels[0].b = map(abs(xyz[_indexThree]) * 100, 0, 192, config->main.minBrightness, config->main.maxBrightness);
        else
            _pixels[0].b = 0;
        //Serial.print("\tb: "); Serial.print(_pixels[0].b);
    }
    else
        _pixels[0].b = map(abs(xyz[_indexThree]) * 100, 0, 192, config->main.minBrightness, config->main.maxBrightness);


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
    for (int i = 1; i < _pixelCount; i++)
    {
        _pixels[i].r = _pixels[0].r;
        _pixels[i].g = _pixels[0].g;
        _pixels[i].b = _pixels[0].b;
        LWUtils.sendColor(_pixels[i]);
    }
    //Serial.println("");
}

void Leg::setPixelCount(byte count, byte half)
{
    _pixelCount = count;
    _half = half;

    if ((_pixelCount % 2) > 0)
        _lower_foot_border = _half - 4;
    else
        _lower_foot_border = _half - 5;
    _upper_foot_border = _half + 4;
}
