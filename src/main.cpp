//these basics are needed until I swap over fully
#include <Arduino.h>
#include <FastLED.h>
//these are network realted
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
//these are mine config last so all the vars are okay
#include <Lpx.h>
#include <Config.h>

//okay we need 2 threads
TaskHandle_t Task0;
TaskHandle_t Task1;

void Task0code(void *pvParameters);
void Task1code(void *pvParameters);

static SemaphoreHandle_t binsem;
static CLpxCommand testSemCommand;

DynamicJsonDocument headRequest(512);

websockets::WebsocketsClient *client = new websockets::WebsocketsClient();

void setup()
{
  //this wraps out config, run here not in thread beacuse it might be needed
  LpxConfig.initConfig();

  binsem = xSemaphoreCreateBinary();
  testSemCommand.strand_index = 0;
  testSemCommand.color[0] = 0;
  testSemCommand.color[1] = 0;
  testSemCommand.color[2] = 0;

  xSemaphoreGive(binsem);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
      Task0code, /* Task function. */
      "Task0",   /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      2,         /* priority of the task */
      &Task0,    /* Task handle to keep track of created task */
      0);        /* pin task to core 0 */

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
      Task1code, /* Task function. */
      "Task1",   /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      2,         /* priority of the task */
      // when priority was set to 100 then network scan would not complete
      &Task1, /* Task handle to keep track of created task */
      1);     /* pin task to core 1 */
}

//kill the vanilla loop
void loop() { vTaskDelete(NULL); }

//NOTE: in here we should only take care of running the lights
void Task0code(void *pvParameters)
{
  for (byte i = 0; i < LpxConfig.CONNECTED_LIGHTS_LENGTH; i++)
  {
    Serial.println(i);
    LpxModes.waterfallRainbow(LpxConfig.CONNECTED_LIGHTS[i], 50);
    LpxModes.off(LpxConfig.CONNECTED_LIGHTS[i]);
  }

  //fill_solid(CONNECTED_LIGHTS[0].strand, CONNECTED_LIGHTS[0].strand_length, CRGB(255, 0, 0));
  //FastLED.show();

  Serial.println("Node_" + (String)LpxConfig.LPX_ID + "_Lit");

  //the loop to mimic the void loop()  @ core 0
  while (true)
  {
    if (xSemaphoreTake(binsem, 500 * portTICK_RATE_MS) == pdTRUE)
    {
      LpxModes.solid(LpxConfig.CONNECTED_LIGHTS[testSemCommand.strand_index], testSemCommand.color[0], testSemCommand.color[1], testSemCommand.color[2]);

      xSemaphoreGive(binsem);
    }
    else
    {
      Serial.println("Failed to read semaphore");
    }

    vTaskDelay(1 / portTICK_PERIOD_MS);

    // CLpxStrip S = CONNECTED_LIGHTS[0];
    // byte M = lpcCommand.Mode;
    // byte R = lpcCommand.Payload[0];
    // byte G = lpcCommand.Payload[1];
    // byte B = lpcCommand.Payload[2];
    // byte De = lpcCommand.Delay;

    // switch (M)
    // {
    // case Off:
    //   LpxModes.off(S);
    //   break;
    // case Solid:
    //   LpxModes.solid(S, R, G, B);
    //   break;
    // case RandomCloudy:
    //   LpxModes.randomCloudyBlobs(S, R, G, B, De);
    //   break;
    // case Flash:
    //   LpxModes.flash(S, R, G, B, De);
    //   break;
    // case Sweep:
    //   //empty bc it is gone
    //   break;
    // case Twinkle:
    //   LpxModes.randomTwinkle(S, R, G, B, De);
    //   break;
    // case RandomTwinkle:
    //   LpxModes.randomTwinkleRainbow(S, De);
    //   break;
    // case TheaterChase:
    //   LpxModes.theaterChase(S, R, G, B, De);
    //   break;
    // case RandomFlash:
    //   LpxModes.randomFlash(S, De);
    //   break;
    // case Chroma:
    //   LpxModes.chroma(S, De);
    //   break;
    // case FadeIn:
    //   LpxModes.fadeIn(S, R, G, B, De);
    //   break;
    // case FadeOut:
    //   LpxModes.fadeOut(S, De);
    //   break;
    // case SuddenFlash:
    //   LpxModes.sudden(S, R, G, B, De);
    //   break;
    // case RandomBreath:
    //   LpxModes.randomBreath(S, De);
    //   break;
    // case Breath:
    //   LpxModes.rgbFadeInAndOut(S, R, G, B, De);
    //   break;
    // case FallingStars:
    //   LpxModes.fallingStars(S, R, G, B, De);
    //   break;
    // case ChristmasChase:
    //   LpxModes.xmasChase(S, De);
    //   break;
    // case Pong:
    //   LpxModes.pong(S, R, G, B, De);
    //   break;
    // case Waterfall:
    //   LpxModes.waterfall(S, R, G, B, De);
    //   break;
    // case Lightning:
    //   LpxModes.lightning(S, R, G, B, De);
    //   break;
    // case Waves:
    //   LpxModes.waves(S, R, G, B, De);
    //   break;
    // case Levels:
    //   LpxModes.levels(S, R, G, B, De);
    //   break;
    // case Rain:
    //   LpxModes.rain(S, R, G, B, De);
    //   break;
    // case SoundSync:
    //   LpxModes.soundsync(S, R, G, B);
    //   break;
    // case Pause:
    //   //empty bc that how it actually pauses
    //   break;
    // default:
    //   break;
    // }
  }
}

//NOTE: defining some of the function used only in the IOT loop here
void semaphoreCommandUpdate(JsonObject header, JsonArray commands)
{
  //make a json helper that parses and gives back the crucial things like color, command, and index
  CLpxCommand c = LpxJson.handleCommandJson(header, commands, LpxConfig);

  if (xSemaphoreTake(binsem, 500 * portTICK_RATE_MS) == pdTRUE)
  {
    testSemCommand = c;

    xSemaphoreGive(binsem);
  }
  else
  {
    Serial.println("Failed to grab semaphore");
  }
}

void onMessageCallback(websockets::WebsocketsMessage message)
{
  Serial.println(millis());

  Serial.print("Got Message: ");
  Serial.println(message.data());

  //make the json into the dock
  deserializeJson(headRequest, message.data());

  const char *requestType = headRequest["header"]["type"];

  switch (LpxJson.packageToEnum(requestType))
  {
  case ELpxPackageTypes::handshake_start:
    client->send(LpxJson.handleHandshakeStartJson(headRequest["header"], headRequest["body"]["request"], LpxConfig));
    break;
  case ELpxPackageTypes::resolve_handshake_event:
    client->send(LpxJson.handleEventSetupJson(headRequest["header"], headRequest["body"]["events"], LpxConfig));
    break;
  case ELpxPackageTypes::lpx_command:
    semaphoreCommandUpdate(headRequest["header"], headRequest["body"]["commands"]);
    break;
  default:
    Serial.println("Case not accounted for");
    break;
  }

  Serial.println(millis());
}

void onEventsCallback(websockets::WebsocketsEvent event, String data)
{
  //really we only need to check if we lose connection in here
  if (event == websockets::WebsocketsEvent::ConnectionClosed)
  {
    while (client != NULL)
    {
      client = NULL;
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
  }
}

//NOTE: in here we should take care of all connection and telemetry
void Task1code(void *pvParameters)
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(LpxConfig.SSID_NAME, LpxConfig.SSID_PASSPHRASE);

  Serial.println("Connecting to: " + (String)LpxConfig.SSID_NAME);

  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.print('.');
  }

  Serial.println(" Connected\nNode IP address is: " + WiFi.localIP().toString());

  if (client->connect(LpxConfig.TARGET_IP, LpxConfig.TARGET_PORT, "/"))
  {
    Serial.println("Connected to LPX Server: " + (String)LpxConfig.TARGET_IP);
  }

  client->onMessage(onMessageCallback);
  client->onEvent(onEventsCallback);

  Serial.println("Node_" + (String)LpxConfig.LPX_ID + "_Online");

  //the loop to mimic the void loop() @ core 1
  while (true)
  {
    if (client != NULL)
    {
      client->poll();
    }
    else
    {
      client = new websockets::WebsocketsClient();

      if (client->connect(LpxConfig.TARGET_IP, LpxConfig.TARGET_PORT, "/"))
      {
        Serial.println("Reconnected after connection was lost");
      }

      client->onMessage(onMessageCallback);
      client->onEvent(onEventsCallback);
    }
  }
}