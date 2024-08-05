#include "BubbleMode.h"

namespace modes {

BubbleMode::BubbleMode(config::Properties *properties, config::LedStripConfig *config) :
		Mode(properties, config) {
	_leadingBubbleBottom = -10;
	_trailingBubbleBottom = config->pixelCount + 10;
	_timer = millis();
}

BubbleMode::~BubbleMode() {
}

const char* BubbleMode::getName() {
	return "bubble";
}

void BubbleMode::frame(bool stepDetected) {
	CHSV backgroundColor, bubbleColor;
	properties->getCHSV(bubbleBackgroundColor, &backgroundColor);
	properties->getCHSV(bubbleBubbleColor, &bubbleColor);
	int width = properties->getInt(bubbleWidth, 4);
	int speed = properties->getInt(bubbleSpeed, 2);
	bool trail = properties->getBool(bubbleTrail, true);

    int dice = 0;
    int leadingBubbleTop, trailingBubbleTop = 0;
    byte variation = min(50, backgroundColor.value);

    if (stepDetected)
    {
        _leadingBubbleBottom = config->pixelHalf;
        _trailingBubbleBottom = config->pixelHalf + 1 + (config->pixelHalf - _leadingBubbleBottom);
    }

    leadingBubbleTop = _leadingBubbleBottom - width + 1;
    trailingBubbleTop = _trailingBubbleBottom + width - 1;

    for (int i = 0; i < config->pixelCount; i++)
    {
        dice = random(0, 250);

        if (((i <= config->pixelHalf) && (i <= _leadingBubbleBottom) && (i >= leadingBubbleTop)) ||
            ((i > config->pixelHalf) && (i >= _trailingBubbleBottom) && (i <= trailingBubbleTop)))
        {
            config->pixels[i] = bubbleColor;
        }
        else if (!trail &&
                 (((i > _leadingBubbleBottom) && (i <= (_leadingBubbleBottom + speed))) ||
                  ((i < _trailingBubbleBottom) && (i >= (_trailingBubbleBottom - speed)))))
        {
            config->pixels[i] = CHSV(backgroundColor.h, backgroundColor.s, backgroundColor.v - random(0, variation));
        }
        else if (dice < 3)
        {
            config->pixels[i] = CHSV(backgroundColor.h, backgroundColor.s, backgroundColor.v - random(0, variation));
        }
    }

    if ((_leadingBubbleBottom >= 0) || (_trailingBubbleBottom <= config->pixelCount))
    {
        _leadingBubbleBottom -= speed;
        _trailingBubbleBottom += speed;
    }
}

} /* namespace modes */
