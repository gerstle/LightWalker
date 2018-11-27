#ifndef MODES_OFFMODE_H_
#define MODES_OFFMODE_H_

#include "Mode.h"

namespace modes {

class OffMode: public modes::Mode {
public:
	OffMode(config::Properties *properties, config::LedStripConfig *config);
	virtual ~OffMode();
	const char* getName();
	void frame(bool stepDetected);
};

} /* namespace config */

#endif /* MODES_OFFMODE_H_ */
