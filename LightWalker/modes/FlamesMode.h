#ifndef MODES_FLAMESMODE_H_
#define MODES_FLAMESMODE_H_

#include "Mode.h"

namespace modes {

class FlamesMode: public Mode {
public:
	FlamesMode(config::Properties *properties, config::LedStripConfig *config);
	virtual ~FlamesMode();
	void frame(bool stepDetected);
	const char* getName();
private:
	unsigned long _currentTime;
	unsigned long _lastStepTime;
	unsigned long _lastFrame;

	unsigned int _cooling = 120;
	unsigned int _sparking = 50;
	// Array of temperature readings at each simulation cell
	byte *_heat;

	CRGB heatColor(uint8_t temperature);
	void fire2012();

	int delay;
};

} /* namespace modes */

#endif /* MODES_FLAMESMODE_H_ */
