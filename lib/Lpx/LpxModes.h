#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "LpxPhysical.h"

//NOTE: this is all lighting mode stuff
enum ELpxModes : byte
{
    Off = 0,
    Solid,          //1
    RandomCloudy,   //2
    Flash,          //3
    Sweep,          //4
    Twinkle,        //5
    RandomTwinkle,  //6
    RandomFlash,    //7
    TheaterChase,   //8
    Chroma,         //9
    FadeIn,         //10
    FadeOut,        //11
    SuddenFlash,    //12
    RandomBreath,   //13
    Breath,         //14
    FallingStars,   //15
    ChristmasChase, //16
    Pong,           //17
    Waterfall,      //18
    Lightning,      //19
    Waves,          //20
    Levels,         //21
    Rain,           //22
    Pause,          //23
    SoundSync,      //24
    enum_size
};

class CLpxModes
{
public:
    static boolean smartDelay(int ms);

    // off - fill a strand of LEDs with 0,0,0
    // Example: off(target);
    static void off(CLpxStrip target);

    // solid - fill a strand of LEDs with R,G,B
    // Example: solid(target, r, g, b);
    static void solid(CLpxStrip target, byte r, byte g, byte b);

    // blobs - creates clusters of R,G,B breathe in and out with delay D
    // Example: randomCloudyBlobs(target, r, g, b, d);
    static void randomCloudyBlobs(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    // flash - fill a strand of LEDs with R,G,B and with delay D
    // Example: flash(target, r, g, b, d);
    static void flash(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void sweep(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void randomTwinkle(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void randomTwinkleRainbow(CLpxStrip target, int delayMs);

    static void randomFlash(CLpxStrip target, int delayMs);

    static void chroma(CLpxStrip target, int delayMs);

    static void theaterChase(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void fadeIn(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void fadeOut(CLpxStrip target, int delayMs);

    static void sudden(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void randomBreath(CLpxStrip target, int delayMs);

    static void rgbFadeInAndOut(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void fallingStars(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void xmasChase(CLpxStrip target, int delayMs);

    static void pong(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void waterfall(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void waterfallRainbow(CLpxStrip target, int delayMs);

    static void lightning(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void waves(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void levels(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void rain(CLpxStrip target, byte r, byte g, byte b, int delayMs);

    static void soundsync(CLpxStrip target, byte r, byte g, byte b);
};

extern CLpxModes LpxModes;
extern bool ONE_TIME_PER_COMMAND;