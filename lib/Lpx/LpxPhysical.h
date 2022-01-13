#pragma once
#include <FastLED.h>

//NOTE: this is for the basic objects that will hold the physical strips and physical io information
class CLpxStrip
{
public:
    static int gIndex;

    TaskHandle_t taskHandle;
    int index;
    int pin;
    int strand_length;
    CRGB *strand;

    CLpxStrip(int p, int l);

    void showStrand();
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

enum ELpxEventTypes : byte
{
    moment = 0x00,
    toggle = 0x01,
    hold = 0x02,
    analog = 0x03,
    unset = 0x04
};

class CLpxIO
{
public:
    int pin;
    EPeripheralType type;
    EPeripheralMode mode;
    ELpxEventTypes event = ELpxEventTypes::unset;

    CLpxIO(int p, EPeripheralMode m, EPeripheralType t);

    bool setEvent(ELpxEventTypes e);
};
