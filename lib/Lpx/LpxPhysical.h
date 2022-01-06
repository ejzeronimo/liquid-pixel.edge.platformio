#pragma once
#include <FastLED.h>

//NOTE: this is for the basic objects that will hold the physical strips and physical io information
class CLpxStrip
{
public:
    int pin;
    int strand_length;
    CRGB *strand;

    CLpxStrip(int p, int l)
    {
        pin = p;
        strand_length = l;
        strand = new CRGB[l];
    }
};

enum EPeripheralType : uint8_t
{
    Analog = 0,
    Digital
};

enum EPeripheralMode : int
{
    Input = 0,
    Input_Pullup,
    Input_Pulldown,
    Output
};

class CLpxIO
{
public:
    EPeripheralType type;
    int pin;
    EPeripheralMode mode;

    CLpxIO(int p, EPeripheralMode m, EPeripheralType t)
    {
        pin = p;
        mode = m;
        type = t;
    }
};
