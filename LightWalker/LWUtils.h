/*****************************************************************************
 * LWUtils.h
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#ifndef LWUtils_h
#define LWUtils_h

#include <TCL.h>
#include "colors.h"

// <gerstle> TX is the arduino's TX... attach to bluetooth RX
// <gerstle> RX is the arduino's RX... attach to bluetooth TX
#define TXPIN 9
#define RXPIN 10

#define AUDIO_PIN 1         // read right channel from analog input 1
#define AUDIO_STROBE_PIN 4  // strobe is attached to digital pin 2
#define AUDIO_RESET_PIN 5   // reset is attached to digital pin 3

class LWUtilsClass
{
    public:
        static void goDark(int pixel_count); 
        static bool debounce(int switchPin, bool lastState);
        static void sendColor(RGB color, byte dimmer);
        static void sendColor(RGB color);
};

extern LWUtilsClass LWUtils;
#endif
