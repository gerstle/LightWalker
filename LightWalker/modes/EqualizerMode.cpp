/*
 * EqualizerMode.cpp
 *
 *  Created on: Nov 26, 2018
 *      Author: cgerstle
 */

#include "EqualizerMode.h"

namespace modes {

EqualizerMode::EqualizerMode(config::Properties *properties, config::LedStripConfig *config) : Mode(properties, config) {
    _perlinsTracker = random16();
    _x = random16();
    _y = random16();
    _lastChangeTimer = millis();
    _direction = 1;
}

EqualizerMode::~EqualizerMode() {
}

void EqualizerMode::setup() {
}

void EqualizerMode::frame(bool stepDetected) {

    float eqLevel = properties->getFloat(Preferences::eqLevel, 0.0); // 0.0 -> 1.0
    int minValue = properties->getInt(eqMinValue, 100);
    EqMode mode = static_cast<EqMode>(properties->getInt(eqMode, EQDoubleRainbow));
    CHSV color = CHSV(180, 255, 255);
    properties->getCHSV(Preferences::eqColor, &color);


    int i = 0;
    int lower_threshold = config->pixelHalf - (eqLevel * config->pixelHalf);
    int upper_threshold = (eqLevel * config->pixelHalf) + config->pixelHalf;
    unsigned long currentTime = millis();

    if (currentTime > (_lastChangeTimer + 400))
    {
        _lastChangeTimer = currentTime;
        if (random8(2) == 0)
            _direction = -1;
        else
            _direction = 1;
    }

    _x += (2 * _direction);
    _y += (2 * _direction);

    for (i = 0; i < config->pixelCount; i++)
    {
        // <cgerstle> this is the "OFF"
        if (((i <= config->pixelHalf) && (i < lower_threshold)) ||
            ((i > config->pixelHalf) && (i > upper_threshold)))
        {
            if (minValue > 0)
            {
                _perlinsTracker += 0.0005;
                byte value = inoise8(_x + i * 20, _y + i * 10, _perlinsTracker);
                // <cgerstle> 0-255, scale down a bit
                value = map(value, 20, 255, 0, minValue);

                if (mode == EQSingleRainbow)
                    config->pixels[i].setHSV(map(i, 0, config->pixelCount, 0, 255), 255, value);
                else if (mode == EQDoubleRainbow)
                {
                    // <gerstle> setting config->pixels[config->pixelCount - 1 - i] to config->pixels[i] would work if half
                    // was actually half... so, run through both sides... :(
                    if (i < config->pixelHalf)
                        config->pixels[i].setHSV(map(i, 0, config->pixelHalf, 0, 255), 255, value);
                    else if (i > config->pixelHalf)
                        config->pixels[i].setHSV(map((config->pixelCount - i - 1), 0, (config->pixelCount - config->pixelHalf - 1), 0, 255), 255, value);
                }
                else
                    config->pixels[i].setHSV(color.hue, color.saturation, value);
            }
            else
                config->pixels[i] = CRGB::Black;
        }
        // <cgerstle> this is the "ON" for rainbow
        else if (mode == EQSingleRainbow)
            config->pixels[i].setHSV(map(i, 0, config->pixelCount, 0, 255), 255, 255);
        else if (mode == EQDoubleRainbow)
        {
            if (i <= config->pixelHalf)
                config->pixels[i].setHSV(map(i, 0, config->pixelHalf, 0, 255), 255, 255);
            else if (i > config->pixelHalf)
                config->pixels[i].setHSV(map((config->pixelCount - i - 1), 0, (config->pixelCount - config->pixelHalf - 1), 0, 255), 255, 255);
        }
        // <gerstle> this is the "ON" for not rainbow
        else
            config->pixels[i] = color;
    }
}

const char* EqualizerMode::getName() {
	return "equalizer";
}

} /* namespace modes */
