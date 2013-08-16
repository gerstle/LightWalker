#include "MainConfigs.h"

MainConfigs::MainConfigs()
{
    minBrightness = 5;
    maxBrightness = 200;
    defaultMode = pulse;
    legsOn = true;
    armsOn = false;

    for (int i = 0; i < COLOR_COUNT; i++)
    {
        minColors[i].r = maxColors[i].r = COLORS[i].r;
        minColors[i].g = maxColors[i].g = COLORS[i].g;
        minColors[i].b = maxColors[i].b = COLORS[i].b;
    }
}
