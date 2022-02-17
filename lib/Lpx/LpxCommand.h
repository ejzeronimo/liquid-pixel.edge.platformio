#pragma once
#include <FastLED.h>

// NOTE: this is all lighting mode stuff
enum ELpxModes : int {
    Off = 0,
    Solid, // 1
    Chroma, // 2

    Flash,       // 3
    RandomFlash, // 4
    SuddenFlash, // 5
    Lightning,   // 6

    CloudyBlobs,   // 7
    Twinkle,       // 8
    RandomTwinkle, // 9

    TheaterChase,     // 10
    DualTheaterChase, // 11
    Rain,             // 12

    FadeIn,       // 13
    FadeOut,      // 14
    Breath,       // 15
    RandomBreath, // 16

    FallingStars,     // 17
    Pong,             // 18
    Waterfall,        // 19
    RainbowWaterfall, // 20

    Waves,  // 21
    Levels, // 22

    Pause, // 23
    enum_size
};

// NOTE: the main focus of this library, this helper has various functions that I define and use everywhere
class CLpxCommand {
public:
    // the strand data
    int strand_index;

    int type;

    bool isTandem;
    int master_strand;
    int masterOffset;

    ELpxModes mode;

    // the colors
    CRGB primary;
    CRGB secondary;

    // other settings
    int delayMs;
};