#pragma once
#include <Arduino.h>

//NOTE: the main focus of this library, this helper has various functions that I define and use everywhere
struct CLpxCommand
{
    byte strand_index;
    byte mode;
    byte color[3];
};
