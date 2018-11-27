#include "ZebraMode.h"
#include "../lw/LWUtils.h"

namespace modes {

ZebraMode::ZebraMode(config::Properties *properties, config::LedStripConfig *config) : Mode(properties, config) {
	_currentTime = _lastSlowTimer = millis();
	_startStage = 0;
	_frame = 0;
	_frames = 4;

    colorOne = CHSV(96, 255, 255);
    colorTwo = CHSV(96, 0, 0);
}

ZebraMode::~ZebraMode() {
}

void ZebraMode::setup() {
    // <gerstle> paint the initial canvas
    byte stage = _startStage;

    for (int i = 0; i < config->pixelHalf; i++) // <gerstle> down
        paintPixel(&stage, i);

    stage = _startStage;
    for (int i = config->pixelCount - 1; i >= config->pixelHalf; i--) // <gerstle> down
        paintPixel(&stage, i);
}

void ZebraMode::frame(bool stepDetected) {
    _currentTime = millis();
    properties->getCHSV(zebraColorOne, &colorOne);
    properties->getCHSV(zebraColorTwo, &colorTwo);

    if (stepDetected)
    {
        _lastSlowTimer = _currentTime;
        _frames = 2;
    }

    if (_frames < 5000)
    {
        byte stage = _startStage;
        for (int i = 0; i < config->pixelHalf; i++) // <gerstle> down
            paintPixel(&stage, i);

        stage = _startStage;
        for (int i = config->pixelCount - 1; i >= config->pixelHalf; i--) // <gerstle> down
            paintPixel(&stage, i);

        if (_currentTime >= (_lastSlowTimer + 400))
        {
            _lastSlowTimer = _currentTime;
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

void ZebraMode::paintPixel(byte (*stage), int index)
{
    if ((*stage) == 0)
        LWUtils.setTransitionColor(&(config->pixels[index]), _frame, _frames, &colorTwo, &colorOne);
    else if (((*stage) >= 1) && ((*stage) <= 4))
        config->pixels[index] = colorTwo;
    else if ((*stage) == 5)
        LWUtils.setTransitionColor(&(config->pixels[index]), _frame, _frames, &colorOne, &colorTwo);
    else if ((*stage) == 6)
        config->pixels[index] = colorOne;

    (*stage)++;
    if ((*stage) >= ZEBRA_LIGHTS)
        (*stage) = 0;
}

const char* ZebraMode::getName() {
	return "zebra";
}

} /* namespace modes */
