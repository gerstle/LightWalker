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

class LWUtilsClass
{
    public:
        static void goDark(int pixel_count); 
        static bool debounce(int switchPin, bool lastState);
//         static void sendColor(RGB color, byte dimmer);
//         static void sendColor(RGB color);
        static void sendColor(byte r, byte g, byte b, byte dimmer);
        static void sendColor(byte r, byte g, byte b);
};

extern LWUtilsClass LWUtils;
#endif
