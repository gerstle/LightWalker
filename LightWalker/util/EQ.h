/*
 * EQ.h
 *
 *  Created on: Apr 19, 2016
 *      Author: cgerstle
 */

#ifndef UTIL_EQ_H_
#define UTIL_EQ_H_

#include "Ema.h"
#include "Arduino.h"

namespace util {

class EQ {
public:
	EQ() : maxEma(20), lastVolume(0), lastMark(millis()), sampleWindow(30) {}
	virtual ~EQ();

	void init();
	float listen();
private:
	Ema maxEma;
	double lastVolume;
	unsigned long lastMark;
	int sampleWindow; // Sample window width in mS (50 mS = 20Hz)
};

} /* namespace util */

#endif /* UTIL_EQ_H_ */
