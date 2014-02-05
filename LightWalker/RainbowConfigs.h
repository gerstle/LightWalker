#ifndef LWRainbowConfigs_h
#define LWRainbowConfigs_h

enum RainbowMode
{
    SingleRainbow,
    DoubleRainbow,
    Rotate,
    Rise
};

class RainbowConfigs
{
    public:
        RainbowMode mode = Rise;
        byte minValue = 40;
        int delay = 25;
};

#endif
