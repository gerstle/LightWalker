#ifndef MODES_GRAVITYMODE_H_
#define MODES_GRAVITYMODE_H_

#include "Mode.h"

namespace modes {

class GravityMode: public Mode {
public:
	GravityMode(config::Properties *properties, config::LedStripConfig *config);
	virtual ~GravityMode();
	const char* getName();
	void frame(bool stepDetected);
private:
	byte _indexOne;
	byte _indexTwo;
	byte _indexThree;
	unsigned long _lastXSwitch;
	unsigned long _lastYSwitch;
	bool _canXSwitch;
	bool _canYSwitch;
};

} /* namespace modes */

#endif /* MODES_GRAVITYMODE_H_ */
