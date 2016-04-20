/*
 * Ema.h
 *
 *  Created on: Mar 5, 2016
 *      Author: cgerstle
 */

#ifndef EMA_H_
#define EMA_H_

class Ema {
public:
	Ema(int samples);
	virtual ~Ema();
	void mark(double value);
	double getCurrentValue();
	bool getInitialized();
private:
	int samples;
	int initialCount;
	bool initialized;
	double ema;
};

#endif /* EMA_H_ */
