/*****************************************************************************
 * colors.h
 *
 * The purpose of this file is to hold color definitions
 *
 * Copyright 2013 Casey Gerstle
 * This program is distributed under the Apache License 2.0.
 * A copy is included in the file LICENSE
 ****************************************************************************/

#ifndef colors_h
#define colors_h

#include "Arduino.h"

class RGB
{
    public: 
        RGB(byte red, byte green, byte blue) : r(red), b(blue), g(green){ }
        RGB() : r(0x00), g(0x00), b(0x00){}
        byte r;
        byte g;
        byte b;
};

const int COLOR_COUNT = 26;
const RGB COLORS[COLOR_COUNT] =
{
    RGB(0x00, 0x00, 0x00),
    RGB(0xff, 0xff, 0xff),
    RGB(0xff, 0x00, 0x00),
    RGB(0xff, 0x3e, 0x00),
    RGB(0xff, 0x7d, 0x00),
    RGB(0xff, 0xbb, 0x00),
    RGB(0xff, 0xff, 0x00),
    RGB(0xbb, 0xff, 0x00),
    RGB(0x7d, 0xff, 0x00),
    RGB(0x3e, 0xff, 0x00),
    RGB(0x00, 0xff, 0x00),
    RGB(0x00, 0xff, 0x3e),
    RGB(0x00, 0xff, 0x7d),
    RGB(0x00, 0xff, 0xbb),
    RGB(0x00, 0xff, 0xff),
    RGB(0x00, 0xbb, 0xff),
    RGB(0x00, 0x7d, 0xff),
    RGB(0x00, 0x3e, 0xff),
    RGB(0x00, 0x00, 0xff),
    RGB(0x3e, 0x00, 0xff),
    RGB(0x7d, 0x00, 0xff),
    RGB(0x88, 0x00, 0xff),
    RGB(0xff, 0x00, 0xff),
    RGB(0xff, 0x00, 0x88),
    RGB(0xff, 0x00, 0x7d),
    RGB(0xff, 0x00, 0x3e)
};

enum Color
{
    BLACK,
    WHITE,
    RED,
    RED_ORANGE,
    ORANGE,
    ORANGE_YELLOW,
    YELLOW,
    YELLOW_GREEN,
    SPRING_GREEN,
    SPRING_GREEN_GREEN,
    GREEN,
    GREEN_TURQUOISE,
    TURQUOISE,
    TURQUOISE_CYAN,
    CYAN,
    CYAN_OCEAN,
    OCEAN,
    OCEAN_BLUE,
    BLUE,
    BLUE_VIOLET,
    VIOLET,
    VIOLET_MAGENTA,
    MAGENTA,
    MAGENT_RASPBERRY,
    RASPBERRY,
    RASPBERRY_RED
};

#endif
