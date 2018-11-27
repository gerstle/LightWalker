#ifndef MODES_BUBBLEMODE_H_
#define MODES_BUBBLEMODE_H_

#include "Mode.h"

namespace modes {

class BubbleMode: public Mode {
public:
	BubbleMode(config::Properties *properties, config::LedStripConfig *config);
	virtual ~BubbleMode();
	void frame(bool stepDetected);
	const char* getName();
private:
	unsigned long _timer;
	int _leadingBubbleBottom;
	int _trailingBubbleBottom;
};

} /* namespace modes */

#endif /* MODES_BUBBLEMODE_H_ */
