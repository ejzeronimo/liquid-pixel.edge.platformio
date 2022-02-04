#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "LpxPhysical.h"
#include "LpxCommand.h"

class CLpxStrip;
class CLpxCommand;

class CLpxModes
{
public:
    static bool smartDelay(int ms, bool taskBool, CLpxStrip *target);

    // off - fill a strand of LEDs with 0,0,0
    // Example: off(target);
    static bool off(CLpxStrip *target, bool oneTimeController);

    // solid - fill a strand of LEDs with R,G,B
    // Example: solid(target, r, g, b);
    static bool solid(CLpxStrip *target, CLpxCommand command, bool oneTimeController);

    // random cloudy blobs - does
    static void randomCloudyBlobs(CLpxStrip *target, CLpxCommand command, bool taskBool);

    // flash - fill a strand of LEDs with R,G,B and with delay D
    // Example: flash(target, r, g, b, d);
    static void flash(CLpxStrip *target, CLpxCommand command, bool taskBool);

    // is gay
    static void sweep(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    // twinkles the color
    static void randomTwinkle(CLpxStrip *target, CLpxCommand command, bool taskBool);

    //done
    static void randomTwinkleRainbow(CLpxStrip *target, CLpxCommand command, bool taskBool);

    //done
    static void randomFlash(CLpxStrip *target, CLpxCommand command, bool taskBool);

    //done
    static void chroma(CLpxStrip *target, CLpxCommand command, bool taskBool);

    //done
    static void theaterChase(CLpxStrip *target, CLpxCommand command, bool taskBool);

    //done
    static bool fadeIn(CLpxStrip *target, CLpxCommand command, bool oneTimeController, bool taskBool);

    //done
    static bool fadeOut(CLpxStrip *target, CLpxCommand command, bool oneTimeController, bool taskBool);

    //done
    static bool sudden(CLpxStrip *target, CLpxCommand command, bool oneTimeController, bool taskBool);

    //done
    static void randomBreath(CLpxStrip *target, CLpxCommand command, bool taskBool);

    // breath - fill a strand of LEDs with R,G,B and fade it in and out
    // Example: rgbFadeInAndOut(target, r, g, b, d);
    static void rgbFadeInAndOut(CLpxStrip *target, CLpxCommand command, bool taskBool);

    // falling stars - like a single chase
    // Example: fallingStars(target, r, g, b, d);
    static void fallingStars(CLpxStrip *target, CLpxCommand command, bool taskBool);

    //done
    static void xmasChase(CLpxStrip *target, CLpxCommand command, bool taskBool);

    //done
    static void pong(CLpxStrip *target, CLpxCommand command, bool taskBool);

    //done
    static bool waterfall(CLpxStrip *target, CLpxCommand command, bool oneTimeController, bool taskBool);

    //we did this one too
    static bool waterfallRainbow(CLpxStrip *target, CLpxCommand command, bool oneTimeController, bool taskBool);

    //done
    static bool lightning(CLpxStrip *target, CLpxCommand command, bool oneTimeController, bool taskBool);

    //done
    static void waves(CLpxStrip *target, CLpxCommand command, bool taskBool);

    //done
    static void levels(CLpxStrip *target, CLpxCommand command, bool taskBool);

    //done
    static void rain(CLpxStrip *target, CLpxCommand command, bool taskBool);

    //done
    static void soundsync(CLpxStrip *target, CLpxCommand command, bool taskBool);
};

extern CLpxModes LpxModes;
extern bool ONE_TIME_PER_COMMAND;