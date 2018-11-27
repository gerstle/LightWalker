#ifndef MODES_ZEBRAMODE_H_
#define MODES_ZEBRAMODE_H_

#include "Mode.h"

namespace modes {

#define ZEBRA_LIGHTS 7

class ZebraMode: public Mode {
public:
	ZebraMode(config::Properties *properties, config::LedStripConfig *config);
	virtual ~ZebraMode();
	void setup();
	void frame(bool stepDetected);
	const char* getName();
private:
	unsigned long _currentTime;
	unsigned long _lastSlowTimer;

	byte _startStage;
	int _frame;
	int _frames;
	CHSV colorOne;
	CHSV colorTwo;

	void paintPixel(byte *stage, int index);
};

} /* namespace modes */

#endif /* MODES_ZEBRAMODE_H_ */
