#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include <LpxPhysical.h>

class CLpxConfig
{
public:
    static const char *LPX_VERSION;
    static const char *LPX_ID;

    static const char *SSID_NAME;
    static const char *SSID_PASSPHRASE;

    static const char *TARGET_IP;
    static const int TARGET_PORT;
    static String TARGET_ID;

    static const int CONNECTED_PERIPHERALS_LENGTH;
    static CLpxIO CONNECTED_PERIPHERALS[];

    static const int CONNECTED_LIGHTS_LENGTH;
    static CLpxStrip CONNECTED_LIGHTS[];

    const void initConfig();
};

//change this based on how many strands you have
extern CLpxConfig LpxConfig;