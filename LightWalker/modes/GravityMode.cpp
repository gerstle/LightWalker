#include "GravityMode.h"

namespace modes {

GravityMode::GravityMode(config::Properties *properties, config::LedStripConfig *config) :
		Mode(properties, config) {
	_indexOne = 0;
	_indexTwo = 1;
	_indexThree = 2;
	_lastXSwitch = _lastYSwitch = millis();
	_canXSwitch = true;
	_canYSwitch = true;
}

GravityMode::~GravityMode() {
}

const char* GravityMode::getName() {
	return "gravity";
}

void GravityMode::frame(bool stepDetected) {
	int minValue = properties->getInt(gravityMinValue, 50);
	bool rotate = properties->getBool(gravityRotate, false);
	double xyz[3];

    if (config->accelerometer == NULL)
    {
		for (int i = 0; i < config->pixelCount; i++)
			if (random8() < 8)
				config->pixels[i] = CHSV(160, 255, max(100, random8()));
    	return;
    }

    config->accelerometer->getAcceleration(xyz);

    if (_indexOne == 2)
    {
        if (xyz[_indexOne] > 0)
            config->pixels[0].r = map(abs(xyz[_indexOne]) * 100, 0, 192, minValue, 255);
        else
            config->pixels[0].r = 0;
    }
    else
        config->pixels[0].r = map(abs(xyz[_indexOne]) * 100, 0, 192, minValue, 255);

    if (_indexTwo == 2)
    {
        if (xyz[_indexTwo] > 0)
            config->pixels[0].g = map(abs(xyz[_indexOne]) * 100, 0, 192, minValue, 255);
        else
            config->pixels[0].g = 0;
    }
    else
        config->pixels[0].g = map(abs(xyz[_indexTwo]) * 100, 0, 192, minValue, 255);

    if (_indexThree == 2)
    {
        if (xyz[_indexThree] > 0)
            config->pixels[0].b = map(abs(xyz[_indexThree]) * 100, 0, 192, minValue, 255);
        else
            config->pixels[0].b = 0;
    }
    else
        config->pixels[0].b = map(abs(xyz[_indexThree]) * 100, 0, 192, minValue, 255);


    if (rotate)
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
        }

        if ((abs(xyz[1]) > 240) && _canYSwitch)
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
        }

        if ((abs(xyz[0]) < 0.3) && (_lastXSwitch >= 600))
        {
            _lastXSwitch = _lastXSwitch - 600;
            _canXSwitch = true;
        }

        if ((abs(xyz[1]) < 0.3) && (_lastYSwitch >= 600))
        {
            _lastYSwitch = _lastYSwitch - 600;
            _canYSwitch = true;
        }
    }

    for (int i = 1; i < config->pixelCount; i++)
    {
        if ((config->pixels[0].r > 10) && (random(0, 20) == 0))
            config->pixels[i].r = config->pixels[0].r - random(0, 10);
        else
            config->pixels[i].r = config->pixels[0].r;

        if ((config->pixels[0].g > 10) && (random(0, 20) == 0))
            config->pixels[i].g = config->pixels[0].g - random(0, 10);
        else
            config->pixels[i].g = config->pixels[0].g;

        if ((config->pixels[0].b > 10) && (random(0, 20) == 0))
            config->pixels[i].b = config->pixels[0].b - random(0, 10);
        else
            config->pixels[i].b = config->pixels[0].b;

    }

}

} /* namespace modes */
