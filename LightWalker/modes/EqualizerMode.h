/*
 * EqualizerMode.h
 *
 *  Created on: Nov 26, 2018
 *      Author: cgerstle
 */

#ifndef MODES_EQUALIZERMODE_H_
#define MODES_EQUALIZERMODE_H_

#include "Mode.h"

namespace modes {

enum EqMode
{
    EQStaticColor,
    EQSingleRainbow,
    EQDoubleRainbow
};

class EqualizerMode: public Mode {
public:
	EqualizerMode(config::Properties *properties, config::LedStripConfig *config);
	virtual ~EqualizerMode();
	void setup();
	void frame(bool stepDetected);
	const char* getName();
private:
        double _perlinsTracker;
        double _x;
        double _y;
        int _direction;
        unsigned long _lastChangeTimer;
};

} /* namespace modes */

#endif /* MODES_EQUALIZERMODE_H_ */
