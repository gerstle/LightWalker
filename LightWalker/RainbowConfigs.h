#ifndef LWRainbowConfigs_h
#define LWRainbowConfigs_h

enum RainbowMode
{
    singleRainbow,
    doubleRainbow,
    rotate,
    rise
};

class RainbowConfigs
{
    public:
        RainbowMode mode = rise;
        byte minValue = 40;
        int delay = 25;
};

#endif
