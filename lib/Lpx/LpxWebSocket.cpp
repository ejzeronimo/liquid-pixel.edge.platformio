#include "LpxWebSocket.h"

//NOTE: this should be anything needed for websocket communication

//things like
//handle

ELpxPackageTypes CLpxJson::packageToEnum(const char *input)
{
    if (strcmp(input, "request_handshake_start") == 0)
        return ELpxPackageTypes::handshake_start;
    if (strcmp(input, "resolve_handshake_event") == 0)
        return ELpxPackageTypes::resolve_handshake_event;
    if (strcmp(input, "lpx_command") == 0)
        return ELpxPackageTypes::lpx_command;

    //default case
    return ELpxPackageTypes::unknown_header;
}

ELpxSchemaTypes CLpxJson::schemaToEnum(const char *input)
{
    if (strcmp(input, "strands") == 0)
        return ELpxSchemaTypes::strands;
    if (strcmp(input, "peripherals") == 0)
        return ELpxSchemaTypes::peripherals;
    if (strcmp(input, "modes") == 0)
        return ELpxSchemaTypes::modes;
    if (strcmp(input, "version") == 0)
        return ELpxSchemaTypes::version;

    //default case
    return ELpxSchemaTypes::unknown_schema;
}

ELpxEventTypes CLpxJson::eventToEnum(const char *input)
{
    if (strcmp(input, "moment") == 0)
        return ELpxEventTypes::moment;

    //default case
    return ELpxEventTypes::unknown_event;
}

String CLpxJson::handleHandshakeStartJson(JsonObject header, JsonArray request, CLpxConfig config)
{
    //define the doc for the response
    DynamicJsonDocument response(512);

    response["header"]["target"] = header["orgin"];
    response["header"]["orgin"] = config.LPX_ID;
    response["header"]["type"] = "return_handshake_start";

    for (byte i = 0; i < request.size() - 1; i++)
    {
        switch (LpxJson.schemaToEnum(request[i]))
        {
        case ELpxSchemaTypes::strands:
        {
            DynamicJsonDocument strandArrayDoc(256);
            JsonArray returnStrands = strandArrayDoc.to<JsonArray>();

            for (byte i = 0; i < config.CONNECTED_LIGHTS_LENGTH; i++)
            {
                DynamicJsonDocument strandDoc(32);
                JsonObject strandObject = strandDoc.to<JsonObject>();

                strandObject["pin"] = config.CONNECTED_LIGHTS[i].pin;
                strandObject["length"] = config.CONNECTED_LIGHTS[i].strand_length;

                returnStrands.add(strandObject);
                strandObject.clear();
            }
            response["body"]["reponse"]["strands"] = returnStrands;
            strandArrayDoc.clear();
        }
        break;
        case ELpxSchemaTypes::peripherals:
        {
            DynamicJsonDocument peripheralArrayDoc(256);
            JsonArray returnPeripherals = peripheralArrayDoc.to<JsonArray>();

            for (byte i = 0; i < config.CONNECTED_PERIPHERALS_LENGTH; i++)
            {
            }

            response["body"]["reponse"]["peripherals"] = returnPeripherals;
            peripheralArrayDoc.clear();
        }
        break;
        case ELpxSchemaTypes::modes:
        {
            response["body"]["reponse"]["modes"] = ELpxModes::enum_size;
        }
        break;
        case ELpxSchemaTypes::version:
        {
            response["body"]["reponse"]["version"] = config.LPX_VERSION;
        }
        break;
        default:
            break;
        }
    }

    String output;
    serializeJson(response, output);
    response.clear();

    return output;
}

String CLpxJson::handleEventSetupJson(JsonObject header, JsonArray events, CLpxConfig config)
{
    for (byte i = 0; i < events.size(); i++)
    {
        //okay for each event passed in we check the pin
        switch (LpxJson.eventToEnum(events[i]["type"]))
        {
        case ELpxEventTypes::moment:
            /* code */
            Serial.print("moment");
            break;
        default:
            break;
        }
    }

    //define the doc for the response
    DynamicJsonDocument response(512);

    //need the header
    response["header"]["target"] = header["orgin"];
    response["header"]["orgin"] = config.LPX_ID;
    response["header"]["type"] = "acknowledge_handshake_event";

    String output;
    serializeJson(response, output);
    response.clear();

    return output;
}

CLpxCommand CLpxJson::handleCommandJson(JsonObject header, JsonArray commands, CLpxConfig config)
{
    CLpxCommand value;

    //we need to check if we are the right box
    if (strcmp(header["target"], config.LPX_ID) == 0)
    {
        value.strand_index = commands[0]["strand_index"];
        value.color[0] = commands[0]["color"][0];
        value.color[1] = commands[0]["color"][1];
        value.color[2] = commands[0]["color"][2];
    }

    return value;
}
