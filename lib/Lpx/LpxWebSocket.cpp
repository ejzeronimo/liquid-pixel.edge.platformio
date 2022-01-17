#include "LpxWebSocket.h"

//NOTE: this should be anything needed for websocket communication

//things like
//handle

String CLpxJson::handleHandshakeStartJson(JsonObject header, JsonArray request, CLpxConfig config)
{
    //define the doc for the response
    DynamicJsonDocument response(512);

    String temp = header["orgin"];
    config.TARGET_ID = temp;

    response["header"]["target"] = header["orgin"];
    response["header"]["orgin"] = config.LPX_ID;
    response["header"]["type"] = ELpxReturnTypes::receive_handshake_start;

    for (byte i = 0; i < request.size(); i++)
    {
        const byte requestType = request[i];

        switch (requestType)
        {
        case ELpxSchemaTypes::strands:
        {
            for (byte i = 0; i < config.CONNECTED_LIGHTS_LENGTH; i++)
            {
                DynamicJsonDocument strandDoc(64);
                JsonObject strandObject = strandDoc.to<JsonObject>();

                strandObject["strand_index"] = i;
                strandObject["pin"] = config.CONNECTED_LIGHTS[i].pin;
                strandObject["length"] = config.CONNECTED_LIGHTS[i].strand_length;

                response["output"]["configuration"]["strands"].add(strandObject);
                strandObject.clear();
            }
        }
        break;
        case ELpxSchemaTypes::peripherals:
        {
            for (byte i = 0; i < config.CONNECTED_PERIPHERALS_LENGTH; i++)
            {
                DynamicJsonDocument peripheralDoc(64);
                JsonObject peripheralObject = peripheralDoc.to<JsonObject>();

                peripheralObject["peripheral_index"] = i;
                peripheralObject["pin"] = config.CONNECTED_PERIPHERALS[i].pin;
                peripheralObject["mode"] = config.CONNECTED_PERIPHERALS[i].mode;
                peripheralObject["type"] = config.CONNECTED_PERIPHERALS[i].type;

                response["output"]["configuration"]["peripherals"].add(peripheralObject);
                peripheralDoc.clear();
            }
        }
        break;
        case ELpxSchemaTypes::modes:
        {
            response["output"]["configuration"]["modes"] = ELpxModes::enum_size;
        }
        break;
        case ELpxSchemaTypes::version:
        {
            response["output"]["configuration"]["version"] = config.LPX_VERSION;
        }
        break;
        case ELpxSchemaTypes::telemetry:
        {
            response["output"]["configuration"]["telemetry"] = "";
        }
        break;
        default:
        {
            response["output"]["configuration"]["errors"].add(requestType);
        }
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
    //define the doc for the response
    DynamicJsonDocument response(512);

    for (byte i = 0; i < events.size(); i++)
    {
        const JsonObject requestEvent = events[i];
        const byte requestIndex = requestEvent["peripheral_index"];

        //okay for each event passed in we check the pin
        if (requestEvent["peripheral_index"] < config.CONNECTED_PERIPHERALS_LENGTH)
        {
            DynamicJsonDocument peripheralDoc(48);
            JsonObject peripheralObject = peripheralDoc.to<JsonObject>();

            peripheralObject["result"] = config.CONNECTED_PERIPHERALS[requestIndex].setEvent(requestEvent["event"]);
            peripheralObject["peripheral_index"] = requestIndex;

            response["result"]["peripherals"].add(peripheralObject);
            peripheralDoc.clear();
        }
        else
        {
            response["result"]["peripherals"]["errors"].add(requestIndex);
        }
    }

    //need the header
    response["header"]["target"] = header["orgin"];
    response["header"]["orgin"] = config.LPX_ID;
    response["header"]["type"] = ELpxReturnTypes::resolve_handshake_event;

    String output;
    serializeJson(response, output);
    response.clear();

    return output;
}

CLpxCommand CLpxJson::handleCommandJson(JsonObject command, CLpxConfig config)
{
    CLpxCommand value;

    value.strand_index = command["strand_index"];
    value.mode = command["mode"];

    for (size_t i = 0; i < 3; i++)
    {
        value.primary[i] = command["primary"][i];
    }

    return value;
}
