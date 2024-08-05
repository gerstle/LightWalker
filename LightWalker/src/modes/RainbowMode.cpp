#include "RainbowMode.h"
#include "../lw/LWUtils.h"

namespace modes {

RainbowMode::RainbowMode(config::Properties *properties, config::LedStripConfig *config) : Mode(properties, config) {
    _lastStartHue = 0;
    _increment = (float)255/(float)config->pixelCount;
    _perlinZ = (double) random8() / (double) 25500;
    _lastNoiseTimer = _lastStepTime = _currentTime = _lastChangeTimer = millis();

    minValue = 40;
    _halfMin = 40;
    delay = 40;
}

RainbowMode::~RainbowMode() {
}

void RainbowMode::frame(bool stepDetected) {
	RainbowModeMode mode = static_cast<RainbowModeMode>(properties->getInt(rainbowMode, Rise));
	minValue = properties->getInt(rainbowMinValue, 40);
	delay = properties->getInt(rainbowDelay, 40);

    _halfMin = (double)minValue / 2;
    _currentTime = millis();

    switch (mode)
    {
        case SingleRainbow:
        case DoubleRainbow:
            if (_currentTime > (_lastNoiseTimer + 50))
            {
                _lastNoiseTimer = _currentTime;
                _perlinZ += 0.001;
            }
            else
            {
                // <gerstle> reset back to previous value so the calcuations work
                // out to be exactly the same again.
                _perlinZ -= (float)config->pixelCount * 0.0005;
            }
            break;
        default:
            break;
    }

    if (stepDetected)
        _lastStepTime = _currentTime;

    switch (mode)
    {
        case SingleRainbow:
            _singleRainbow();
            break;
        case DoubleRainbow:
            _doubleRainbow();
            break;
        case Rotate:
            _rotate();
            break;
        case Rise:
            _rise();
            break;
    }
}

const char* RainbowMode::getName() {
	return "rainbow";
}

void RainbowMode::_singleRainbow()
{
    for (int i = 0; i < config->pixelCount; i++)
        config->pixels[i].setHSV(map(i, 0, config->pixelCount, 0, 255), 255, _getValue(i));
}

void RainbowMode::_doubleRainbow()
{
    for (int i = 0; i < config->pixelHalf; i++)
        config->pixels[i].setHSV(map(i, 0, config->pixelHalf, 0, 255), 255, _getValue(i));

    // <gerstle> setting config->pixels[config->pixelCount - 1 - i] to config->pixels[i] would work if half
    // was actually half... so, run through both sides... :(
    for (int i = config->pixelHalf; i < config->pixelCount; i++)
        config->pixels[i].setHSV(map((config->pixelCount - i - 1), 0, (config->pixelCount - config->pixelHalf - 1), 0, 255), 255, _getValue(i));
}

void RainbowMode::_rotate()
{
    if (_currentTime > (_lastChangeTimer + delay))
    {
        _lastChangeTimer = _currentTime;
        _perlinZ += 0.001;

        // <gerstle>  this increment shows the entire hue spectrum on a leg
        // if you go less than that, not all the colors show all the time, kinda nice.
        for (int i = 0; i < config->pixelCount; i++)
            config->pixels[i].setHSV(_lastStartHue + (((float)i) * (_increment * .6)), 255, _getValue(i));

        // <gerstle> let it roll under
        _lastStartHue--;
    }
}

void RainbowMode::_rise()
{
    if (_currentTime > (_lastChangeTimer + delay))
    {
        _lastChangeTimer = _currentTime;
        _perlinZ += 0.001;

        for (int i = 0; i < config->pixelHalf; i++)
            config->pixels[i].setHSV(_lastStartHue + (((float)i) * _increment), 255, _getValue(i));

        // <gerstle> setting config->pixels[config->pixelCount - 1 - i] to config->pixels[i] would work if half
        // was actually half... so, run through both sides... :(
        for (int i = config->pixelHalf; i < config->pixelCount; i++)
            config->pixels[i].setHSV(_lastStartHue + (((float)(config->pixelCount - i - 1)) * _increment), 255, _getValue(i));

        // <gerstle> let it roll over
        _lastStartHue++;
    }
}

byte RainbowMode::_getValue(int i)
{
    float value;

    if (_currentTime > (_lastStepTime + 1000))
    {
        _perlinZ += 0.0015;
        double noiseModifier = LWUtils.perlinNoise(_perlinZ + sin(i /  4), cos(_perlinZ), _perlinZ);
        //return (noiseModifier * (double)_halfMin) + (double)_halfMin * 1.5;
        return map((noiseModifier * (double)127) + (double)128, 0, 255, 30, (double)255.0 * (double) 0.6);
    }

    // <cgerstle> value based on location... brighter at the bottom
    if (i < config->pixelHalf)
        value = ((float) i / (float) config->pixelHalf) * 255;
    else
        value = ((float) (config->pixelCount - 1 - i) / (float) (config->pixelCount - config->pixelHalf)) * 255;
    value = max(minValue, value);

    // <gerstle> now dim over time
    value = max(minValue, value - (((float)(_currentTime - _lastStepTime) / (float)1000) * (float)(value - minValue)));

    return value;
}

} /* namespace modes */
