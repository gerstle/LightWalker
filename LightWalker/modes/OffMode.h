/*
 * OffMode.h
 *
 *  Created on: Nov 25, 2018
 *      Author: cgerstle
 */

#ifndef MODES_OFFMODE_H_
#define MODES_OFFMODE_H_

#include "Mode.h"

namespace modes {

class OffMode: public modes::Mode {
public:
	OffMode(config::Properties *properties, config::LedStripConfig *config);
	virtual ~OffMode();
	void frame(bool stepDetected);
	const char* getName();
};

} /* namespace config */

#endif /* MODES_OFFMODE_H_ */
