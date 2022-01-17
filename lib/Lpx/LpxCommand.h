#pragma once

//NOTE: this is all lighting mode stuff
enum ELpxModes : int
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

//NOTE: the main focus of this library, this helper has various functions that I define and use everywhere
class CLpxCommand
{
public:
    //the strand data
    int strand_index;

    bool isTandem;
    int master_strand;
    int masterOffset;

    ELpxModes mode;

    //the colors
    int primary[3];
    int secondary[3];

    //other settings
    int delayMs;
};