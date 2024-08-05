/*
 * Ema.cpp
 *
 *  Created on: Mar 5, 2016
 *      Author: cgerstle
 */

#include "Ema.h"

Ema::Ema(int samples) {
	this->samples = samples;
	initialized = false;
	initialCount = 0;
	ema = 0;
}

Ema::~Ema() {
}

void Ema::mark(double value) {
    if (!initialized) {
        if (initialCount < samples) {
            ema += value;
            initialCount++;
        }

        if (initialCount == samples) {
            ema = ema / samples;
        }
    }

    if (initialCount >= samples) {
        ema += (value - ema) * (2.0 / (samples + 1));
        initialized = true;
    }
}

double Ema::getCurrentValue() {
	if (initialized)
		return ema;
	else
		return 0.0;
}

bool Ema::getInitialized() {
	return initialized;
}
