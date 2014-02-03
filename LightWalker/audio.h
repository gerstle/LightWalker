
#pragma once

#ifndef audio_h
#define audio_h

#include "LWConfigs.h"

void equalizerBaseline(LWConfigs *configs);
void spectrumSetup(LWConfigs *configs);
void samplingCallback();
void samplingBegin();
bool samplingIsDone();
void windowMean(float* magnitudes, int lowBin, int highBin, float* windowMean, float* otherMean);
int frequencyToBin(float frequency);
float equalizerListen(LWConfigs *configs);

#endif
