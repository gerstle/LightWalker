#include "SparkleMode.h"

namespace modes {

SparkleMode::SparkleMode(config::Properties *properties, config::LedStripConfig *config) :
		Mode(properties, config) {
	_state = Off;
	_lastChangeTimer = millis();

	if ((config->pixelCount % 2) > 0)
		_lowerFootBorder = config->pixelHalf - 4;
	else
		_lowerFootBorder = config->pixelHalf - 5;
	_upperFootBorder = config->pixelHalf + 4;

	_currentTime = millis();

	sparkleColor = CHSV(192, 255, 255);
	footFlashColor = CHSV(0, 255, 0);
	minValue = 50;
	flashLength = 500;
	sparkleLength = 550;
	fadeRate = 8;
}

SparkleMode::~SparkleMode() {
}

void SparkleMode::frame(bool stepDetected) {
	properties->getCHSV(sparkleSparkleColor, &sparkleColor);
	properties->getCHSV(sparkleFootFlashColor, &footFlashColor);
	minValue = properties->getInt(sparkleMinValue, 50);
	flashLength = properties->getInt(sparkleFlashLength, 500);
	sparkleLength = properties->getInt(sparkleSparkleLength, 550);
	fadeRate = properties->getInt(sparkleFadeRate, 8);

	_currentTime = millis();

	if (stepDetected)
		_flash();
	else
		_sameStatus();
}

const char* SparkleMode::getName() {
	return "sparkle";
}

void SparkleMode::_setState(SparkleState newState)
{
    if (_state != newState)
    {
//          if (_channel == ADXL_ONE)
//             Serial.println("resetting timer");
        _lastChangeTimer = _currentTime;
    }

//     if (_channel == ADXL_ONE)
//     {
//         Serial.print("_setState: "); Serial.println(newState);
//     }
    _state = newState;
}

void SparkleMode::_sameStatus()
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

        default:
        	break;
    }
}

// <gerstle> shimmer happens all the time.
void SparkleMode::_shimmer()
{
//     if (_channel == ADXL_ONE)
//     {
//         Serial.print("shimmering "); Serial.println(_legName);
//     }

    int dice = 0;
    for (int i = 0; i < config->pixelCount; i++)
    {
        dice = random(0, 100);
        // <cgerstle> leave the middle ones on bright...
        if ((i <= (config->pixelHalf + 1)) && (i >= (config->pixelHalf - 1)))
        {
            config->pixels[i] = sparkleColor;
        }
        // <cgerstle> sparkle the rest if minbrightness is on
        else if (dice == 0)
        {
            config->pixels[i].setHSV(sparkleColor.h, sparkleColor.s, max(minValue, (sparkleColor.v - random(minValue, 255)) / 2));
        }
    }

    _setState(Off);
}

// <gerstle> footdown -> flash -> sparkle -> fade -> off/shimmer
void SparkleMode::_flash()
{
//     if (_channel == ADXL_ONE)
//     {
//         Serial.print("flashing "); Serial.println(_legName);
//     }

    for (int i = 0; i < config->pixelCount; i++)
    {
        if ((i < _lowerFootBorder) || (i > _upperFootBorder))
            config->pixels[i] = CRGB::Black;
        else
        {
            // <gerstle> Let's make sure this is cranked up...
            config->pixels[i].setHSV(footFlashColor.h, footFlashColor.s, 255);
        }
    }
    _setState(Flash);
}

// <gerstle> footdown -> flash -> sparkle -> fade -> off/shimmer
void SparkleMode::_sparkle()
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
        sparkleValue = ((float) i / (float) config->pixelHalf);
        rand = (float)random(0, min(4, sparkleValue * 100)) / 100;
        sparkleValue = (sparkleValue - rand) * 255;
        //Serial.print(i); Serial.print("\t"); Serial.print(sparkleValue); Serial.print("\t"); Serial.println(rand);
        config->pixels[i].setHSV(sparkleColor.h, sparkleColor.s, sparkleValue);
    }

    //foot
    //Serial.print(_currentTime); Serial.print("\t");
    //Serial.print(_lastChangeTimer); Serial.print("\t");
    //Serial.println(_config->sparkle.flashLength);
    CHSV *footColor = &footFlashColor;
    if (_currentTime >= (_lastChangeTimer + flashLength))
        footColor = &sparkleColor;

    for (int i = _lowerFootBorder; i <= _upperFootBorder; i++)
        config->pixels[i] = *footColor;

    // trailing leg
    for (int i = _upperFootBorder + 1; i < config->pixelCount; i++)
    {
        sparkleValue = ((float) (config->pixelCount - 1 - i) / (float) config->pixelHalf);
        if (random(0, 30) == 0)
            rand = (float)random(0, min(20, sparkleValue * 100)) / 100;
        sparkleValue = (sparkleValue - rand) * 256;
        //Serial.print(i); Serial.print("\t"); Serial.print(sparkleValue); Serial.print("\t"); Serial.println(rand);
        config->pixels[i].setHSV(sparkleColor.h, sparkleColor.s, sparkleValue);
    }

    // <gerstle> if we're past our sparkle time, start fading
    if (_currentTime >= (_lastChangeTimer + sparkleLength))
    {
        for (int i = _lowerFootBorder; i <= _upperFootBorder; i++)
                config->pixels[i] = sparkleColor;
         //Serial.println("setting to fade");
        _setState(Fade);
    }
    else
    {
         //Serial.println("setting to sparkle");
         _setState(SparkleSparkle);
    }
}

// <gerstle> footdown -> flash -> sparkle -> fade -> off/shimmer
void SparkleMode::_fade()
{
    int still_fading = 1;
    for (int i = 0; i < config->pixelCount; i++)
        if (((config->pixels[i].r - fadeRate) > 0) ||
            ((config->pixels[i].g - fadeRate) > 0) ||
            ((config->pixels[i].b - fadeRate) > 0))
        {
            config->pixels[i].fadeToBlackBy(fadeRate);
            still_fading++;
        }
    //if (_channel == ADXL_ONE)
    //{
        //Serial.print("\t\tstill fading"); Serial.println(still_fading);
    //}

    if ((minValue > 0) && (still_fading > (config->pixelCount / 3)))
        _setState(Fade);
    else if ((minValue == 0) && (still_fading))
        _setState(Fade);
    else
        _setState(Off);
}

} /* namespace modes */
