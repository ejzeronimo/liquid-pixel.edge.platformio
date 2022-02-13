#pragma once
#include "LpxCommand.h"
#include "LpxPhysical.h"
#include <FastLED.h>
#include <functional>

class CLpxStrip;
class CLpxCommand;

class CLpxModes {
public:
    // off - fill a strand of LEDs with nothing
    // uses & sets one time per strand
    static void off(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // solid - fill a strand of LEDs with color
    // uses & sets one time per strand
    static void solid(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // chroma - slowly shifts a rainbow across the strand
    static void chroma(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // flash - fill a strand of LEDs with color then turns them on and off
    static void flash(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // random flash - flash but with a random color each cycle
    static void randomFlash(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // sudden flash - turns on the strand waits then turns it off once
    // uses & sets one time per strand
    static void suddenFlash(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // lightning - flashes three times d/6 d/4 d/2
    // uses & sets one time per strand
    static void lightning(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // cloudy blobs - chooses random LEDs and fills tand dims them
    static void cloudyBlobs(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // twinkle - like random cloudy but with single pixels
    static void twinkle(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // random twinkle - twinkle with random colors
    static void randomTwinkle(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // theater chase - does a classic theater chase with one color
    static void theaterChase(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // dueal theater chase - theater chase with two colors
    // NOTE: TODO: currently does primary and red color
    static void dualTheaterChase(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // rain - like a theater chase + falling stars
    static void rain(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // fade in - goes from current color to new color
    // uses & sets one time per strand
    static void fadeIn(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // fade out - goes from current color to off
    // uses & sets one time per strand
    static void fadeOut(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // breath - fill a strand of LEDs and fades it in and out
    static void breath(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // random breath - breath but a random color each cycle
    static void randomBreath(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // falling stars - a "star goes from orgin to tip"
    static void fallingStars(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // pong - falling stars but it bounces once it hits the end of the strand
    static void pong(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // waterfall - wipes the strand to a color
    // uses & sets one time per strand
    static void waterfall(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // rainbow waterfall - waterfall but does a rainbow wipe
    // uses & sets one time per strand
    static void rainbowWaterfall(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    //  waves - falling stars with a longer trail and some noise
    static void waves(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);

    // levels - inverse falling stars but it moves up & down a random amount
    static void levels(CLpxStrip *target, CLpxCommand command, std::function<bool(int)> delayCallback);
};

extern CLpxModes LpxModes;