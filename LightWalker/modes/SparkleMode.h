#ifndef MODES_SPARKLEMODE_H_
#define MODES_SPARKLEMODE_H_

#include "Mode.h"

namespace modes {

enum SparkleState {
	None, Flash, SparkleSparkle, Fade, Off
};

class SparkleMode: public Mode {
public:
	SparkleMode(config::Properties *properties, config::LedStripConfig *config);
	virtual ~SparkleMode();
	void frame(bool stepDetected);
	const char* getName();
private:
	SparkleState _state;
	unsigned long _currentTime;
	unsigned long _lastChangeTimer;

	int _lowerFootBorder;
	int _upperFootBorder;

	CHSV sparkleColor;
	CHSV footFlashColor;
	int minValue;
	int flashLength;
	int sparkleLength;
	int fadeRate;

	void _setState(SparkleState newState);
	void _sameStatus();
	void _flash();
	void _shimmer();
	void _sparkle();
	void _fade();
};

} /* namespace modes */

#endif /* MODES_SPARKLEMODE_H_ */
