#include "PulseConfigs.h"

PulseConfigs::PulseConfigs()
{
    minBrightness = 0;
    maxBrightness = 200;
    minPulseTime = 1000;
    maxPulseTime = 5000;
    randomColor = false;
    syncLegs = false;
    color.r = 0;
    color.g = 0;
    color.b = 255;
}