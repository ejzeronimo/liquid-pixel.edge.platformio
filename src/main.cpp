// these basics are needed until I swap over fully
#include <Arduino.h>
// these are network realted
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
#include <vector>
// these are mine config last so all the vars are okay
#include <Config.h>
#include <Lpx.h>
// these are to disable brownout
#include <soc/rtc_cntl_reg.h>
#include <soc/soc.h>

#include <esp_websocket_client.h>

// okay we need 2 threads
TaskHandle_t Task0;
TaskHandle_t Task1;

// NOTE: hoisted for compile time
void Task0code(void *pvParameters);
void Task1code(void *pvParameters);

void TaskInputCode(void *instancePointer);
void TaskOutputCode(void *instancePointer);
void TaskStrandCode(void *instancePointer);

static SemaphoreHandle_t binsem;
static std::vector<CLpxCommand> globalCommands;

// crucial bc its how the leds and io know when to kill the current thing and move on
static bool wsData = false;

DynamicJsonDocument headRequest(512);

websockets::WebsocketsClient *client = new websockets::WebsocketsClient();

void setup() {
    // this wraps out config, run here not in thread beacuse it might be needed
    LpxConfig.initConfig();

    // here we should make the tasks for the strands
    for (size_t i = 0; i < LpxConfig.CONNECTED_LIGHTS_LENGTH; i++) {
        CLpxStrip *s = &LpxConfig.CONNECTED_LIGHTS[i];

        // okay so lets try making a new task that has the sole job of running this command
        xTaskCreatePinnedToCore(
            TaskStrandCode,
            ("StripTask" + i),
            8000,
            (void *)s,
            1,
            &LpxConfig.CONNECTED_LIGHTS[i].taskHandle,
            1);
    }

    // here we should make the tasks for the inputs
    for (size_t i = 0; i < LpxConfig.CONNECTED_PERIPHERALS_LENGTH; i++) {
        CLpxIO *io = &LpxConfig.CONNECTED_PERIPHERALS[i];

        if (io->mode != EPeripheralMode::Output) {
            // if we are an input
            xTaskCreatePinnedToCore(
                TaskInputCode,
                ("InputTask" + i),
                8000,
                (void *)io,
                2,
                &LpxConfig.CONNECTED_PERIPHERALS[i].taskHandle,
                1);
        } else {
            // if we are an output
            xTaskCreatePinnedToCore(
                TaskOutputCode,
                ("OutputTask" + i),
                8000,
                (void *)io,
                2,
                &LpxConfig.CONNECTED_PERIPHERALS[i].taskHandle,
                1);
        }
    }

    binsem = xSemaphoreCreateBinary();
    xSemaphoreGive(binsem);

    setCpuFrequencyMhz(240);

    // create a task that will be executed in the Task0code() function, with priority 1 and executed on core 0
    xTaskCreatePinnedToCore(
        Task0code,   /* Task function. */
        "MainTask0", /* name of task. */
        10000,       /* Stack size of task */
        NULL,        /* parameter of the task */
        0,           /* priority of the task */
        &Task0,      /* Task handle to keep track of created task */
        0);          /* pin task to core 0 */
}

// kill the vanilla loop
void loop() {
    vTaskDelete(NULL);
    return;
}

void onMessageCallback(websockets::WebsocketsMessage message) {
    wsData = true;
    Serial.print("Got Message: ");
    Serial.println(message.data());

    // make the json into the dock
    deserializeJson(headRequest, message.data());

    const byte requestType = headRequest["header"]["type"];

    switch (requestType) {
    case ELpxPackageTypes::invoke_handshake_start: {
        client->send(LpxJson.handleHandshakeStartJson(headRequest["header"], headRequest["request"]["configuration"], LpxConfig));
    } break;
    case ELpxPackageTypes::register_handshake_event: {
        JsonArray obj = headRequest["deployment"]["peripherals"];

        // we iterate through the array and config the IO
        for (int i = 0; i < obj.size(); i++) {
            JsonObject ioObj = obj[i];
            int index = ioObj["peripheral_index"];

            vTaskSuspend(LpxConfig.CONNECTED_PERIPHERALS[index].taskHandle);

            LpxConfig.CONNECTED_PERIPHERALS[index].setEvent(ioObj["event"]);

            vTaskResume(LpxConfig.CONNECTED_PERIPHERALS[index].taskHandle);
        }

        client->send(LpxJson.handleEventSetupJson(headRequest["header"], headRequest["deployment"]["peripherals"], LpxConfig));
    } break;
    case ELpxPackageTypes::invoke_handshake_end: {
        DynamicJsonDocument response(512);

        // make our header
        response["header"]["target"] = LpxConfig.TARGET_ID;
        response["header"]["orgin"] = LpxConfig.LPX_ID;
        response["header"]["type"] = ELpxReturnTypes::receive_handshake_end;

        String output;
        serializeJson(response, output);
        client->send(output);
        response.clear();
    } break;
    case ELpxPackageTypes::invoke_lpx_command: {
        JsonArray commands = headRequest["request"]["commands"];

        for (int i = 0; i < commands.size(); i++) {
            CLpxCommand temp = LpxJson.handleCommandJson(commands[i], LpxConfig);
            // for each command set the right value
            if (commands[1]["type"] == 0) {
                LpxConfig.CONNECTED_LIGHTS[temp.strand_index].commandAsync(temp);
            } else {
                LpxConfig.CONNECTED_PERIPHERALS[temp.strand_index].commandAsync(temp);
            }
        }
    } break;
    default:
        break;
    }

    wsData = false;
}

void onEventsCallback(websockets::WebsocketsEvent event, String data) {
    // really we only need to check if we lose connection in here
    if (event == websockets::WebsocketsEvent::ConnectionClosed) {
        while (client != NULL) {
            client = NULL;
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}

// NOTE: in here we should take care of all connection and telemetry
void Task0code(void *pvParameters) {
    Serial.begin(115200);

    // disable brownout detector
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    WiFi.mode(WIFI_STA);
    WiFi.begin(LpxConfig.SSID_NAME, LpxConfig.SSID_PASSPHRASE);

    // renanble brownout detector
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1);

    Serial.println("Connecting to: " + (String)LpxConfig.SSID_NAME);

    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        Serial.print('.');
    }

    Serial.println(" Connected\nNode IP address is: " + WiFi.localIP().toString());
    Serial.println("Connecting to LPX Server");

    while (client->connect(LpxConfig.TARGET_IP, LpxConfig.TARGET_PORT, "/") == false) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        Serial.print('.');
    }

    Serial.println(" Connected\nLPX Server: " + (String)LpxConfig.TARGET_IP);

    client->onMessage(onMessageCallback);
    client->onEvent(onEventsCallback);

    Serial.println("Node_" + (String)LpxConfig.LPX_ID + "_Online");

    // the loop to mimic the void loop() @ core 0
    while (true) {
        if (client != NULL) {
            client->poll();
        } else {
            client = new websockets::WebsocketsClient();

            Serial.println("Reconnecting to LPX Server");

            while (!client->connect(LpxConfig.TARGET_IP, LpxConfig.TARGET_PORT, "/")) {
                Serial.print('.');
            }

            Serial.println(" Reconnected");

            client->onMessage(onMessageCallback);
            client->onEvent(onEventsCallback);
        }
    }
}

// NOTE: the new tasks for all IO - one for in, one for out, and one for lights, will add more as more features get added
bool webSocketDelayCallback(int ms = -1) {
    if (ms == -1) {
        // default case
        return !wsData;
    } else {
        long startMs = millis(); // get the time

        // currently blocking which is gay but not much of a choice
        while (millis() < (startMs + ms)) {

            if (wsData) {
                Serial.println("killing early");
                return false;
            }
        }

        return true;
    }
}

void TaskInputCode(void *instancePointer) {
    // get ourselves
    CLpxIO *io = (CLpxIO *)instancePointer;

    while (true) {

        // once we have an event we run this
        switch (io->event) {
            // NOTE: fire on active
        case ELpxEventTypes::moment:
            if (digitalRead(io->pin) == 1 && !io->localEventTriggered) {
                io->eventTrigger = true;
                io->localEventTriggered = true;
            } else if (digitalRead(io->pin) == 0 && io->localEventTriggered) {
                io->localEventTriggered = false;
            }
            break;

            // NOTE: fire on rise and fall
        case ELpxEventTypes::toggle:
            if (digitalRead(io->pin) == 1 && !io->localEventTriggered) {
                io->eventTrigger = true;
                io->localEventTriggered = true;
            } else if (digitalRead(io->pin) == 0 && io->localEventTriggered) {
                io->eventTrigger = true;
                io->localEventTriggered = false;
            }
            break;
        case ELpxEventTypes::hold:
            if (digitalRead(io->pin) == 1 && !io->localEventTriggered) {
                io->eventTrigger = true;
                io->localEventTriggered = true;
            } else if (digitalRead(io->pin) == 0 && io->localEventTriggered) {
                io->eventTrigger = true;
                io->localEventTriggered = false;
            }
            break;

            // NOTE: fire on change
        case ELpxEventTypes::analog:
            // TODO: make analog work (its really noisy)
            break;
        default:
            // its unset
            break;
        }

        // takes care of the data event
        if (io->eventTrigger) {
            DynamicJsonDocument response(512);

            // make our header
            response["header"]["target"] = LpxConfig.TARGET_ID;
            response["header"]["orgin"] = LpxConfig.LPX_ID;
            response["header"]["type"] = ELpxReturnTypes::invoke_io_event;

            DynamicJsonDocument returnDoc(128);
            JsonObject returnObject = returnDoc.to<JsonObject>();

            returnObject["peripheral_index"] = io->index;
            returnObject["event"] = io->event;

            switch (io->event) {
            case ELpxEventTypes::moment:
                io->eventTrigger = false;
                break;
            case ELpxEventTypes::toggle:
                returnObject["state"] = io->localEventTriggered;
                io->eventTrigger = false;
                break;
            case ELpxEventTypes::hold:
                returnObject["state"] = io->localEventTriggered;
                io->eventTrigger = false;
                break;
            case ELpxEventTypes::analog:
                // TODO:
                break;
            default:
                // its unset
                break;
            }
            response["request"]["events"][0] = returnDoc;
            returnDoc.clear();

            String output;
            serializeJson(response, output);
            client->send(output);
            response.clear();

            Serial.println("event fired");
        }

        vTaskDelay(1 / portTICK_RATE_MS);
    }
}

void TaskOutputCode(void *instancePointer) {
    // get ourselves
    CLpxIO *io = (CLpxIO *)instancePointer;

    while (1) {
        for (int i = 0; i < io->commandList.size(); i++) {
            // store the current command
            CLpxCommand com = io->commandList.at(0);

            // run the switch
            switch (com.mode) {
            case 0:
                if (!io->eventTrigger) {
                    digitalWrite(io->pin, LOW);

                    io->eventTrigger = true;
                }
                break;
            case 1:
                if (!io->eventTrigger) {
                    Serial.println("high");
                    digitalWrite(io->pin, HIGH);

                    if (!webSocketDelayCallback(com.delayMs)) {
                        break;
                    }

                    digitalWrite(io->pin, LOW);

                    io->eventTrigger = true;
                }
                break;
            default:
                break;
            }

            // remove command from cue
            if (io->commandList.size() > 1) {
                io->commandList.erase(io->commandList.begin());
                io->eventTrigger = false;
            }
        }

        vTaskDelay(1 / portTICK_RATE_MS);
    }
}

void TaskStrandCode(void *instancePointer) {
    // get the object that made this task & make a pointer to the stuff we need
    CLpxStrip *obj = (CLpxStrip *)instancePointer;

    while (true) {
        for (int i = 0; i < obj->commandList.size(); i++) {
            // store the current command
            CLpxCommand com = obj->commandList.at(0);

            // run the switch
            switch (com.mode) {
                // NOTE: solid modes
            case ELpxModes::Off:
                LpxModes.off(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::Solid:
                LpxModes.solid(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::Chroma:
                LpxModes.chroma(obj, com, webSocketDelayCallback);
                break;

                // NOTE: flashing modes
            case ELpxModes::Flash:
                LpxModes.flash(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::RandomFlash:
                LpxModes.randomFlash(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::SuddenFlash:
                LpxModes.suddenFlash(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::Lightning:
                LpxModes.lightning(obj, com, webSocketDelayCallback);
                break;

                // NOTE: twinkle modes
            case ELpxModes::CloudyBlobs:
                LpxModes.cloudyBlobs(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::Twinkle:
                LpxModes.twinkle(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::RandomTwinkle:
                LpxModes.randomTwinkle(obj, com, webSocketDelayCallback);
                break;

                // NOTE: the theater chase modes
            case ELpxModes::TheaterChase:
                LpxModes.theaterChase(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::DualTheaterChase:
                LpxModes.dualTheaterChase(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::Rain:
                LpxModes.rain(obj, com, webSocketDelayCallback);
                break;

                // NOTE: breath modes
            case ELpxModes::FadeIn:
                LpxModes.fadeIn(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::FadeOut:
                LpxModes.fadeOut(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::Breath:
                LpxModes.breath(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::RandomBreath:
                LpxModes.randomBreath(obj, com, webSocketDelayCallback);
                break;

                // NOTE: falling-star type modes
            case ELpxModes::FallingStars:
                LpxModes.fallingStars(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::Pong:
                LpxModes.pong(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::Waterfall:
                LpxModes.waterfall(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::RainbowWaterfall:
                LpxModes.rainbowWaterfall(obj, com, webSocketDelayCallback);
                break;

                // NOTE: modified falling-star type modes
            case ELpxModes::Waves:
                LpxModes.waves(obj, com, webSocketDelayCallback);
                break;
            case ELpxModes::Levels:
                LpxModes.levels(obj, com, webSocketDelayCallback);
                break;

                // NOTE: control modes
            case ELpxModes::Pause:
                // empty bc that how it actually pauses
                break;
            default:
                break;
            }

            // remove command from cue
            if (obj->commandList.size() > 1) {
                obj->commandList.erase(obj->commandList.begin());
                obj->oneTimePerStrand = false;
            }
        }

        vTaskDelay(1 / portTICK_RATE_MS);
    }
}