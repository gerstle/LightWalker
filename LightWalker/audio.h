
#pragma once

#ifndef audio_h
#define audio_h

#include "LWConfigs.h"

void equalizerBaseline(LWConfigs *configs);
float equalizerListen(LWConfigs *configs);
void audioSetup();

#ifdef __MK20DX256__
void spectrumSetup(LWConfigs *configs);
void samplingCallback();
void samplingBegin();
bool samplingIsDone();
void windowMean(float* magnitudes, int lowBin, int highBin, float* windowMean, float* otherMean);
int frequencyToBin(float frequency);
#else
#endif

#endif
