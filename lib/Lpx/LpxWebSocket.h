#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

#include <Config.h>
#include "LpxModes.h"
#include "LpxCommand.h"

enum ELpxPackageTypes : byte
{
    handshake_start = 0,
    resolve_handshake_event,
    lpx_command,
    unknown_header
};

enum ELpxSchemaTypes : byte
{
    strands = 0,
    peripherals,
    modes,
    version,
    unknown_schema
};

enum ELpxEventTypes : byte
{
    moment = 0,
    toggle,
    hold,
    value,
    unknown_event
};

class CLpxJson
{
public:
    static ELpxPackageTypes packageToEnum(const char *input);

    static ELpxSchemaTypes schemaToEnum(const char *input);

    static ELpxEventTypes eventToEnum(const char *input);

    static String handleHandshakeStartJson(JsonObject header, JsonArray request, CLpxConfig config);

    static String handleEventSetupJson(JsonObject header, JsonArray events, CLpxConfig config);

    static CLpxCommand handleCommandJson(JsonObject header, JsonArray commands, CLpxConfig config);
};

extern CLpxJson LpxJson;