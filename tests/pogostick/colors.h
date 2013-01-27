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

struct RGB {
    RGB(byte red, byte green, byte blue) : r(red), b(blue), g(green){}
    byte r;
    byte g;
    byte b;
};

const RGB COLOR_RED(0xff, 0x00, 0x00);
const RGB COLOR_GREEN(0x00, 0xff, 0x00);
const RGB COLOR_BLUE(0x00, 0x00, 0xff);
const RGB COLOR_WHITE(0xff, 0xff, 0xff);
const RGB COLOR_BLACK(0x00, 0x00, 0x00);
#endif
