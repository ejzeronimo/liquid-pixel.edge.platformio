#pragma once
#include <FastLED.h>
#include <functional>
#include "LpxPhysical.h"
#include "LpxCommand.h"

class CLpxStrip;
class CLpxCommand;

class CLpxModes
{
public:
    // off - fill a strand of LEDs with 0,0,0
    // Example: off(target);
    static void off(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // solid - fill a strand of LEDs with R,G,B
    // Example: solid(target, r, g, b);
    static void solid(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // random cloudy blobs - does
    static void randomCloudyBlobs(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // flash - fill a strand of LEDs with R,G,B and with delay D
    // Example: flash(target, r, g, b, d);
    static void flash(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // is gay
    static void sweep(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // twinkles the color
    static void randomTwinkle(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void randomTwinkleRainbow(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void randomFlash(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void chroma(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void theaterChase(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void fadeIn(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void fadeOut(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void sudden(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void randomBreath(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // breath - fill a strand of LEDs with R,G,B and fade it in and out
    // Example: rgbFadeInAndOut(target, r, g, b, d);
    static void rgbFadeInAndOut(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // falling stars - like a single chase
    // Example: fallingStars(target, r, g, b, d);
    static void fallingStars(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void xmasChase(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void pong(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void waterfall(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // we did this one too
    static void waterfallRainbow(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static bool lightning(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void waves(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void levels(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void rain(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // done
    static void soundsync(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);
};

extern CLpxModes LpxModes;