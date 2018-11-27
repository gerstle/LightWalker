/*
 * EQ.cpp
 *
 *  Created on: Apr 19, 2016
 *      Author: cgerstle
 */

#include "EQ.h"

#include "../config/LWConfigs.h"
#include <Arduino.h>

namespace util {

EQ::~EQ() {
}

void EQ::init() {
    pinMode(MIC_PIN, INPUT);
    pinMode(GAIN_PIN, OUTPUT);
    digitalWrite(GAIN_PIN, HIGH);

	//Serial.println("========================== setting 50");
//	pinMode(GAIN_PIN, OUTPUT);
//	digitalWrite(GAIN_PIN, LOW);

	//Serial.println("========================== setting 40");
	pinMode(GAIN_PIN, OUTPUT);
	digitalWrite(GAIN_PIN, HIGH);

	//Serial.println("========================== setting 60");
//	pinMode(GAIN_PIN, INPUT);

}

float EQ::listen() {
    unsigned long startMillis = millis();  // Start of sample window
    unsigned int peakToPeak = 0;   // peak-to-peak level
    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;
	unsigned int sample;

    // collect data
    while (millis() - startMillis < sampleWindow)
    {
        sample = analogRead(MIC_PIN);
        if (sample < 1024)  // toss out spurious readings
        {
        	if (sample > signalMax)
                signalMax = sample;  // save just the max levels
            else if (sample < signalMin)
                signalMin = sample;  // save just the min levels
        }
    }

    peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
    double volts = (peakToPeak * 3.3) / 1024;
    volts = volts * 100;
    volts = map(volts, (double)0, (double)200, (double)0, (double)700) / (double) 100;

    double volume = pow((double)2, volts);

    if (lastVolume > maxEma.getCurrentValue() || !maxEma.getInitialized() || ((millis() - lastMark) > 1000))
    {
        maxEma.mark((double) lastVolume * 1.5);
        lastMark = millis();
    }

//    for (int i = 0; i < volume || i < maxEma.getCurrentValue(); i++)
//    {
//    	if (i == (int)maxEma.getCurrentValue())
//    		Serial.print("|");
//    	else if (i < volume)
//			Serial.print("-");
//    	else
//    		Serial.print(" ");
//    }
//    Serial.println();

    lastVolume = volume;

//    Serial.print(volume); Serial.print("\t"); Serial.print(maxEma.getCurrentValue()); Serial.print("\t"); Serial.println(volume / maxEma.getCurrentValue());
    return volume / maxEma.getCurrentValue();
}

} /* namespace util */
