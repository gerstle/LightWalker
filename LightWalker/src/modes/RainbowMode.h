#ifndef MODES_RAINBOWMODE_H_
#define MODES_RAINBOWMODE_H_

#include "Mode.h"

namespace modes {

enum RainbowModeMode
{
    SingleRainbow,
    DoubleRainbow,
    Rotate,
    Rise
};

class RainbowMode: public Mode {
public:
	RainbowMode(config::Properties *properties, config::LedStripConfig *config);
	virtual ~RainbowMode();
	void frame(bool stepDetected);
	const char* getName();
private:
	unsigned long _currentTime;
	unsigned long _lastStepTime;
	unsigned long _lastChangeTimer;
	byte _lastStartHue;
	float _increment;

	void _singleRainbow();
	void _doubleRainbow();
	void _rotate();
	void _rise();
	byte _getValue(int i);

	double _perlinZ;
	unsigned long _lastNoiseTimer;

	byte _halfMin;
	byte minValue;
	int delay;
};

} /* namespace modes */

#endif /* MODES_RAINBOWMODE_H_ */
