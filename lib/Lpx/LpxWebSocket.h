#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FastLED.h>


#include "Config.h"
#include "LpxModes.h"
#include "LpxPhysical.h"
#include "LpxCommand.h"

enum ELpxPackageTypes : byte
{
    invoke_handshake_start = 0x00,
    register_handshake_event = 0x01,
    invoke_handshake_end = 0x02,
    invoke_lpx_command = 0x03
};

enum ELpxReturnTypes : byte
{
    receive_handshake_start = 0x00,
    resolve_handshake_event = 0x01,
    receive_handshake_end = 0x02,
    receive_lpx_command = 0x03,
    invoke_io_event = 0x04,
};

enum ELpxSchemaTypes : byte
{
    strands = 0x00,
    peripherals = 0x01,
    modes = 0x02,
    version = 0x03,
    telemetry = 0x04
};

class CLpxJson
{
public:
    static String handleHandshakeStartJson(JsonObject header, JsonArray request, CLpxConfig config);

    static String handleEventSetupJson(JsonObject header, JsonArray events, CLpxConfig config);

    static CLpxCommand handleCommandJson(JsonObject command, CLpxConfig config);
};

extern CLpxJson LpxJson;