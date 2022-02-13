// these basics are needed until I swap over fully
#include <Arduino.h>
// these are network realted
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <vector>
// these are mine config last so all the vars are okay
#include <Lpx.h>
#include <Config.h>
// these are to disable brownout
#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>

#include <esp_websocket_client.h>

// okay we need 2 threads
TaskHandle_t Task0;
TaskHandle_t Task1;

// NOTE: hoisted for compile time
void Task0code(void *pvParameters);
void Task1code(void *pvParameters);
void TaskStrandCode(void *instancePointer);

static SemaphoreHandle_t binsem;
static std::vector<CLpxCommand> globalCommands;

// crucial bc its how the leds and io know when to kill the current thing and move on
static bool wsData = false;

DynamicJsonDocument headRequest(512);

websockets::WebsocketsClient *client = new websockets::WebsocketsClient();

void setup()
{
  // this wraps out config, run here not in thread beacuse it might be needed
  LpxConfig.initConfig();

  // NOTE: here we should make the tasks for the IO/strands
  for (size_t i = 0; i < LpxConfig.CONNECTED_LIGHTS_LENGTH; i++)
  {

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
void loop()
{
  vTaskDelete(NULL);
  return;
}

// TODO: this function abstraction makes it harder to read and slower, look into refactoring our main code to make it more readble

// NOTE: defining some of the function used only in the IOT loop here
void invokeLocalCommand(JsonObject header, JsonArray commands)
{
  for (int i = 0; i < commands.size(); i++)
  {
    // String y = "starting command " + millis();
    // Serial.println(y);

    // for each command set the right value
    CLpxCommand temp = LpxJson.handleCommandJson(commands[i], LpxConfig);
    LpxConfig.CONNECTED_LIGHTS[temp.strand_index].commandAsync(temp);
  }
}

void onMessageCallback(websockets::WebsocketsMessage message)
{
  wsData = true;
  Serial.print("Got Message: ");
  Serial.println(message.data());

  // make the json into the dock
  deserializeJson(headRequest, message.data());

  const byte requestType = headRequest["header"]["type"];

  switch (requestType)
  {
  case ELpxPackageTypes::invoke_handshake_start:
    client->send(LpxJson.handleHandshakeStartJson(headRequest["header"], headRequest["request"]["configuration"], LpxConfig));
    break;
  case ELpxPackageTypes::register_handshake_event:
  {
    JsonArray obj = headRequest["deployment"]["peripherals"];
    JsonArray *ptr = &obj;

    // create a task that will be executed in the Task1code() function, with priority 0 and executed on core 1
    xTaskCreatePinnedToCore(
        Task1code,    /* Task function. */
        "EventTask1", /* name of task. */
        10000,        /* Stack size of task */
        (void *)ptr,  /* parameter of the task */
        2,            /* priority of the task */
        &Task1,       /* Task handle to keep track of created task */
        0);           /* pin task to core 0 */

    client->send(LpxJson.handleEventSetupJson(headRequest["header"], headRequest["deployment"]["peripherals"], LpxConfig));
  }
  break;
  case ELpxPackageTypes::invoke_handshake_end:
  {
    DynamicJsonDocument response(512);

    // make our header
    response["header"]["target"] = LpxConfig.TARGET_ID;
    response["header"]["orgin"] = LpxConfig.LPX_ID;
    response["header"]["type"] = ELpxReturnTypes::receive_handshake_end;

    String output;
    serializeJson(response, output);
    client->send(output);
    response.clear();
  }
  break;
  case ELpxPackageTypes::invoke_lpx_command:
    invokeLocalCommand(headRequest["header"], headRequest["request"]["commands"]);
    break;
  default:
    Serial.println("Case not accounted for");
    break;
  }

  wsData = false;
}

void onEventsCallback(websockets::WebsocketsEvent event, String data)
{
  // really we only need to check if we lose connection in here
  if (event == websockets::WebsocketsEvent::ConnectionClosed)
  {
    while (client != NULL)
    {
      client = NULL;
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
  }
}

// NOTE: in here we should take care of all connection and telemetry
void Task0code(void *pvParameters)
{
  Serial.begin(115200);

  // disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  WiFi.mode(WIFI_STA);
  WiFi.begin(LpxConfig.SSID_NAME, LpxConfig.SSID_PASSPHRASE);

  // renanble brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1);

  Serial.println("Connecting to: " + (String)LpxConfig.SSID_NAME);

  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(100 / portTICK_PERIOD_MS);
    Serial.print('.');
  }

  Serial.println(" Connected\nNode IP address is: " + WiFi.localIP().toString());
  Serial.println("Connecting to LPX Server");

  while (client->connect(LpxConfig.TARGET_IP, LpxConfig.TARGET_PORT, "/") == false)
  {
    vTaskDelay(100 / portTICK_PERIOD_MS);
    Serial.print('.');
  }

  Serial.println(" Connected\nLPX Server: " + (String)LpxConfig.TARGET_IP);

  client->onMessage(onMessageCallback);
  client->onEvent(onEventsCallback);

  Serial.println("Node_" + (String)LpxConfig.LPX_ID + "_Online");

  // the loop to mimic the void loop() @ core 0
  while (true)
  {
    if (client != NULL)
    {
      client->poll();
    }
    else
    {
      client = new websockets::WebsocketsClient();

      Serial.println("Reconnecting to LPX Server");

      while (!client->connect(LpxConfig.TARGET_IP, LpxConfig.TARGET_PORT, "/"))
      {
        Serial.print('.');
      }

      Serial.println(" Reconnected");

      client->onMessage(onMessageCallback);
      client->onEvent(onEventsCallback);
    }

    // vTaskDelay(1 / portTICK_RATE_MS);
    // delayMicroseconds(500);
  }
}

// NOTE: in here we should take care of all event triggers for IO
void Task1code(void *pvParameters)

{
  // take the jsonarray in
  JsonArray arr = *((JsonArray *)pvParameters);

  // we iterate through the array and config the IO
  for (int i = 0; i < arr.size(); i++)
  {
    JsonObject obj = arr[i];
    int index = obj["peripheral_index"];

    LpxConfig.CONNECTED_PERIPHERALS[index].mode = obj["type"];
  }

  Serial.println("Node_" + (String)LpxConfig.LPX_ID + "_Events_Online");

  // the loop to mimic the void loop() @ core 1
  while (true)
  {
    for (int i = 0; i < LpxConfig.CONNECTED_PERIPHERALS_LENGTH; i++)
    {
      CLpxIO *io = &LpxConfig.CONNECTED_PERIPHERALS[i];

      switch (io->event)
      {
      case ELpxEventTypes::moment:
      {
        if (digitalRead(io->pin) == 1 && !io->localEventTriggered)
        {
          io->eventTrigger = true;
          io->localEventTriggered = true;
        }
        else if (digitalRead(io->pin) == 0 && io->localEventTriggered)
        {
          io->localEventTriggered = false;
        }
      }
      break;
      case ELpxEventTypes::toggle:
      {
        if (digitalRead(io->pin) == 1 && !io->localEventTriggered)
        {
          io->eventTrigger = true;
          io->localEventTriggered = true;
        }
        else if (digitalRead(io->pin) == 0 && io->localEventTriggered)
        {
          io->eventTrigger = true;
          io->localEventTriggered = false;
        }
      }
      break;
      case ELpxEventTypes::hold:
      {
        if (digitalRead(io->pin) == 1 && !io->localEventTriggered)
        {
          io->eventTrigger = true;
          io->localEventTriggered = true;
        }
        else if (digitalRead(io->pin) == 0 && io->localEventTriggered)
        {
          io->eventTrigger = true;
          io->localEventTriggered = false;
        }
      }
      break;
      case ELpxEventTypes::analog:
      {
        // TODO: make analog work (its really noisy)
        //  int temp = analogRead(io->pin);
        //  if (temp != io->localEventValue)
        //  {
        //    io->localEventValue = temp;
        //  }
      }
      break;
      default:
        // its unset
        break;
      }

      // takes care of the data event
      if (io->eventTrigger)
      {
        DynamicJsonDocument response(512);

        // make our header
        response["header"]["target"] = LpxConfig.TARGET_ID;
        response["header"]["orgin"] = LpxConfig.LPX_ID;
        response["header"]["type"] = ELpxReturnTypes::invoke_io_event;

        DynamicJsonDocument returnDoc(128);
        JsonObject returnObject = returnDoc.to<JsonObject>();

        returnObject["peripheral_index"] = i;
        returnObject["event"] = io->event;

        switch (io->event)
        {
        case ELpxEventTypes::moment:
        {
          // Serial.print("moment");
          io->eventTrigger = false;
        }
        break;
        case ELpxEventTypes::toggle:
        {
          // Serial.print("toggle");
          // Serial.print(io->localEventTriggered);
          returnObject["state"] = io->localEventTriggered;

          io->eventTrigger = false;
        }
        break;
        case ELpxEventTypes::hold:
        {
          if (io->localEventTriggered)
          {
            // Serial.print("holddown");
            returnObject["state"] = "down";
          }
          else
          {
            returnObject["state"] = "up";
            // Serial.print("holdrelease");
          }

          io->eventTrigger = false;
        }
        break;
        case ELpxEventTypes::analog:
        {
          // TODO:
        }
        break;
        default:
          // its unset
          break;
        }
        response["request"]["events"][0] = returnDoc;
        returnDoc.clear();

        String output;
        serializeJson(response, output);
        // Serial.println(output);

        // suspend out main task so we can send (maybe)
        // vTaskSuspend(Task0);
        client->send(output);
        // vTaskResume(Task0);

        response.clear();
      }
    }
    vTaskDelay(1 / portTICK_RATE_MS);
    // delayMicroseconds(100);
  }
}

// NOTE: the new tasks for all IO - one for in, one for out, and one for lights, will add more as more features get added
void TaskInputCode(void *taskParams)
{
  client->available();

  while (1)
  {
    // do nothing for now
  }
}

void TaskOutputCode(void *taskParams)
{
  client->available();

  while (1)
  {
    // do nothing for now
  }
}

// NOTE: function that acts as a delay/check callback for modes - needed to keep the modes from getting stuck
bool webSocketDelayCallback(int ms = -1)
{
  if (ms == -1)
  {
    // default case
    return !wsData;
  }
  else
  {
    long startMs = millis(); // get the time

    // currently blocking which is gay but not much of a choice
    while (millis() < (startMs + ms))
    {
      if (wsData)
      {
        Serial.println("killing early");
        return false;
      }
    }

    return true;
  }
}

void TaskStrandCode(void *instancePointer)
{

  // get the object that made this task & make a pointer to the stuff we need
  CLpxStrip *obj = (CLpxStrip *)instancePointer;

  while (true)
  {

    for (int i = 0; i < obj->commandList.size(); i++)
    {
      // store the current command
      CLpxCommand com = obj->commandList.at(0);

      // run the switch
      switch (c.mode)
      {
      case ELpxModes::Off:
        LpxModes.off(obj, com, webSocketDelayCallback);
        break;
      case ELpxModes::Solid:
        LpxModes.solid(obj, com, webSocketDelayCallback);
        break;
      case ELpxModes::RandomCloudy:
        // TODO: here!!!
        LpxModes.randomCloudyBlobs(obj, c, oneTime);
        break;
      case ELpxModes::Flash:
        LpxModes.flash(obj, c, handler);
        break;
      case ELpxModes::Sweep:
        // empty bc it is gone
        break;
      case ELpxModes::Twinkle:
        LpxModes.randomTwinkle(obj, c, handler);
        break;
      case ELpxModes::RandomTwinkle:
        LpxModes.randomTwinkleRainbow(obj, c, handler);
        break;
      case ELpxModes::TheaterChase:
        LpxModes.theaterChase(obj, c, handler);
        break;
      case ELpxModes::RandomFlash:
        LpxModes.randomFlash(obj, c, handler);
        break;
      case ELpxModes::Chroma:
        LpxModes.chroma(obj, c, handler);
        break;
      case ELpxModes::FadeIn:
        oneTime = LpxModes.fadeIn(obj, c, oneTime, handler);
        break;
      case ELpxModes::FadeOut:
        oneTime = LpxModes.fadeOut(obj, c, oneTime, handler);
        break;
      case ELpxModes::SuddenFlash:
        oneTime = LpxModes.sudden(obj, c, oneTime, handler);
        break;
      case ELpxModes::RandomBreath:
        LpxModes.randomBreath(obj, c, handler);
        break;
      case ELpxModes::Breath:
        LpxModes.rgbFadeInAndOut(obj, c, handler);
        break;
      case ELpxModes::FallingStars:
        LpxModes.fallingStars(obj, c, handler);
        break;
      case ELpxModes::ChristmasChase:
        LpxModes.xmasChase(obj, c, handler);
        break;
      case ELpxModes::Pong:
        LpxModes.pong(obj, c, handler);
        break;
      case ELpxModes::Waterfall:
        oneTime = LpxModes.waterfall(obj, c, oneTime, handler);
        break;
      case ELpxModes::Lightning:
        oneTime = LpxModes.lightning(obj, c, oneTime, handler);
        break;
      case ELpxModes::Waves:
        LpxModes.waves(obj, c, handler);
        break;
      case ELpxModes::Levels:
        LpxModes.levels(obj, c, handler);
        break;
      case ELpxModes::Rain:
        LpxModes.rain(obj, com, webSocketDelayCallback);
        break;
      case ELpxModes::Pause:
        // empty bc that how it actually pauses
        break;
      case ELpxModes::SoundSync:
        // empty bc its gone
        break;
      case ELpxModes::RainbowWaterfall:
        oneTime = LpxModes.waterfallRainbow(obj, c, oneTime, handler);
        break;
      default:
        break;
      }

      // remove command from cue
      if (obj->commandList.size() > 1)
      {
        obj->commandList.erase(obj->commandList.begin());
        oneTime = true;
      }

      if (handler != obj->taskStateControl)
      {
        handler = obj->taskStateControl;
        oneTime = true;
      }
      // else if ((c.mode != obj->commandList.at(0).mode) && (c.delayMs != obj->commandList.at(0).delayMs) && (c.primary != obj->commandList.at(0).primary))
      // {
      //     oneTime = true;
      // }
    }

    vTaskDelay(1 / portTICK_RATE_MS);
  }
}