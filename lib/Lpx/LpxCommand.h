#pragma once
#include <Arduino.h>
#include "LpxModes.h"
#include <Config.h>

//NOTE: the main focus of this library, this helper has various functions that I define and use everywhere
class CLpxCommand
{
public:
    //the strand data
    byte strand_indicies[MAX_SEM_COMMANDS];

    ELpxModes mode;

    //the colors
    byte primary[3];
    byte secondary[3];
};
