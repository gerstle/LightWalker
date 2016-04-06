
#pragma once

#ifndef audio_h
#define audio_h

#include "../config/LWConfigs.h"

void equalizerBaseline(LWConfigs *configs);
float equalizerListen(LWConfigs *configs);
void audioSetup();

void spectrumSetup(LWConfigs *configs);
void samplingCallback();
void samplingBegin();
bool samplingIsDone();
void windowMean(float* magnitudes, int lowBin, int highBin, float* windowMean, float* otherMean);
int frequencyToBin(float frequency);

#endif
