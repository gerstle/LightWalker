#ifndef MODES_MODE_H_
#define MODES_MODE_H_

#include "../config/LedStripConfig.h"
#include "../config/Properties.h"

namespace modes {

class Mode {
public:
	Mode(config::Properties *properties, config::LedStripConfig *config);
	virtual ~Mode();

	virtual void setup();
	virtual void frame(bool stepDetected);
	virtual const char* getName();

    protected:
        unsigned long lastChangeTimer;
        bool stepDetected;
        config::Properties *properties;
        config::LedStripConfig *config;
};

} /* namespace mode */

#endif /* MODES_MODE_H_ */
