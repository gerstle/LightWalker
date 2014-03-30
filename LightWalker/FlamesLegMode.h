#ifndef FlamesLegMode_h
#define FlamesLegMode_h

#include "LegMode.h"

class FlamesLegMode : public LegMode
{
    public:
        FlamesLegMode() : _lastStepTime(millis()), _lastFrame(millis()), min_height(2) {}

        virtual void setup(LWConfigs *c, char *n, int i2c_channel, ADXL345 *adxl, byte count, byte half, CRGB *p);
        virtual void frame();

    private:
        void _flame(int start, int count, bool invert, int *current_height, int *next_height, int *accent_low_end, int *accent_high_end, bool *reset_accent);

        unsigned long _currentTime;
        unsigned long _lastStepTime;
        unsigned long _lastFrame;

        byte min_height;

        int front_current_height;
        int front_next_height;
        int front_accent_low_end;
        int front_accent_high_end;
        bool front_reset_accent;

        int back_current_height;
        int back_next_height;
        int back_accent_low_end;
        int back_accent_high_end;
        bool back_reset_accent;
};

#endif
