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
        RainbowConfigs() : mode(Rise), minValue(40), delay(25) {}

        RainbowMode mode;
        byte minValue;
        int delay;
};

#endif
