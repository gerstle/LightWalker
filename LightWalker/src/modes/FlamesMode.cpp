#include "FlamesMode.h"

namespace modes {

FlamesMode::FlamesMode(config::Properties *properties, config::LedStripConfig *config) : Mode(properties, config) {
	delay = 32;
	_heat = new byte[config->pixelCount];
	_lastFrame = _lastStepTime = _currentTime = millis();
}

FlamesMode::~FlamesMode() {
	delete [] _heat;
}

const char* FlamesMode::getName() {
	return "flames";
}

void FlamesMode::frame(bool stepDetected) {
    _currentTime = millis();

    delay = properties->getInt(flamesDelay, 32);

    if (stepDetected)
        _lastStepTime = _currentTime;

    if (_currentTime >= (_lastFrame + delay))
    {
        _lastFrame = _currentTime;
        fire2012();
    }
}

void FlamesMode::fire2012()
{
    // Step 1.  Cool down every cell a little
    for (int i = 0; i < config->pixelCount; i++)
        _heat[i] = qsub8(_heat[i],  random8(0, ((_cooling * 10) / config->pixelCount) + 2));

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = config->pixelCount - 3; k > config->pixelHalf; k--)
        _heat[k] = (_heat[k - 1] + _heat[k - 2] + _heat[k - 2] ) / 3;

    for (int k = 0; k < (config->pixelHalf - 3); k++)
        _heat[k] = (_heat[k + 1] + _heat[k + 2] + _heat[k + 2] ) / 3;

    // Step 3.  Randomly ignite new 'sparks' of _heat near the bottom
    //if (random8() < SPARKING)
    if ((_currentTime - _lastStepTime) < 200)
    {
        for (int i = config->pixelHalf - 6; i < (config->pixelHalf + 6); i++)
            _heat[i] = 230;
    }
    else if (random8() < _sparking)
    {
        int y_back = random8(config->pixelHalf, config->pixelHalf + 6);
        int y_front = random8(config->pixelHalf - 6, config->pixelHalf);
        _heat[y_back] = qadd8(_heat[y_back], random8(160, 256));
        _heat[y_front] = qadd8(_heat[y_front], random8(160, 256));
    }

    // Step 4.  Map from _heat cells to LED colors
    for (int j = 0; j < config->pixelCount; j++)
        config->pixels[j] = heatColor(_heat[j]);
}


// CRGB HeatColor( uint8_t temperature)
// [to be included in the forthcoming FastLED v2.1]
//
// Approximates a 'black body radiation' spectrum for
// a given '_heat' level.  This is useful for animations of 'fire'.
// Heat is specified as an arbitrary scale from 0 (cool) to 255 (hot).
// This is NOT a chromatically correct 'black body radiation'
// spectrum, but it's surprisingly close, and it's extremely fast and small.
//
// On AVR/Arduino, this typically takes around 70 bytes of program memory,
// versus 768 bytes for a full 256-entry RGB lookup table.
CRGB FlamesMode::heatColor(uint8_t temperature)
{
    CRGB heatcolor;

    // Scale '_heat' down from 0-255 to 0-191,
    // which can then be easily divided into three
    // equal 'thirds' of 64 units each.
    uint8_t t192 = scale8_video(temperature, 192);

    // calculate a value that ramps up from
    // zero to 255 in each 'third' of the scale.
    uint8_t heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2; // scale up to 0..252

    // now figure out which third of the spectrum we're in:
    if (t192 & 0x80)
    {
        // we're in the hottest third
        heatcolor.r = 255; // full red
        heatcolor.g = 255; // full green
        heatcolor.b = heatramp; // ramp up blue
    }
    else if(t192 & 0x40)
    {
        // we're in the middle third
        heatcolor.r = 255; // full red
        heatcolor.g = heatramp; // ramp up green
        heatcolor.b = 0; // no blue
    }
    else
    {
        // we're in the coolest third
        heatcolor.r = heatramp; // ramp up red
        heatcolor.g = 0; // no green
        heatcolor.b = 0; // no blue
    }

    return heatcolor;
}

} /* namespace modes */
