//these basics are needed until I swap over fully
#include <Arduino.h>
#include <FastLED.h>
//these are network realted
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <vector>
//these are mine config last so all the vars are okay
#include <Lpx.h>
#include <Config.h>

//okay we need 2 threads
TaskHandle_t Task0;
TaskHandle_t Task1;

void Task0code(void *pvParameters);
void Task1code(void *pvParameters);

static SemaphoreHandle_t binsem;
static std::vector<CLpxCommand> globalCommands;

std::vector<CLpxCommand> localCommands;

DynamicJsonDocument headRequest(512);

websockets::WebsocketsClient *client = new websockets::WebsocketsClient();

void setup()
{
  //this wraps out config, run here not in thread beacuse it might be needed
  LpxConfig.initConfig();

  binsem = xSemaphoreCreateBinary();
  xSemaphoreGive(binsem);

  setCpuFrequencyMhz(240);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
      Task0code, /* Task function. */
      "Task0",   /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      0,         /* priority of the task */
      &Task0,    /* Task handle to keep track of created task */
      0);        /* pin task to core 0 */

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
      Task1code, /* Task function. */
      "Task1",   /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      0,         /* priority of the task */
      &Task1, /* Task handle to keep track of created task */
      1);     /* pin task to core 1 */
}

//kill the vanilla loop
void loop()
{
  vTaskDelete(NULL);
  return;
}

//NOTE: defining some of the function used only in the IOT loop here
void semaphoreCommandUpdate(JsonObject header, JsonArray commands)
{
  if (xSemaphoreTake(binsem, 500 * portTICK_RATE_MS) == pdTRUE)
  {

    for (int i = 0; i < commands.size(); i++)
    {
      //for each command set the right value
      CLpxCommand temp = LpxJson.handleCommandJson(commands[i], LpxConfig);
      globalCommands.push_back(temp);
    }

    xSemaphoreGive(binsem);
  }
  else
  {
    Serial.println("Failed to grab semaphore");
  }
}

void onMessageCallback(websockets::WebsocketsMessage message)
{
  Serial.print("Got Message: ");
  Serial.println(message.data());

  //make the json into the dock
  deserializeJson(headRequest, message.data());

  const byte requestType = headRequest["header"]["type"];

  switch (requestType)
  {
  case ELpxPackageTypes::invoke_handshake_start:
    client->send(LpxJson.handleHandshakeStartJson(headRequest["header"], headRequest["request"]["configuration"], LpxConfig));
    break;
  case ELpxPackageTypes::register_handshake_event:
    client->send(LpxJson.handleEventSetupJson(headRequest["header"], headRequest["deployment"]["peripherals"], LpxConfig));
    break;
  case ELpxPackageTypes::invoke_handshake_end:
    // semaphoreCommandUpdate(headRequest["header"], headRequest["body"]["commands"]);
    //quietly end handshake
    break;
  case ELpxPackageTypes::invoke_lpx_command:
    semaphoreCommandUpdate(headRequest["header"], headRequest["request"]["commands"]);
    break;
  default:
    Serial.println("Case not accounted for");
    break;
  }
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
void Task0code(void *pvParameters)
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(LpxConfig.SSID_NAME, LpxConfig.SSID_PASSPHRASE);

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

    delay(1); //not using vTaskDelay(1 / portTICK_PERIOD_MS)
  }
}

//NOTE: in here we should only take care of running the lights
void Task1code(void *pvParameters)
{
  for (byte i = 0; i < LpxConfig.CONNECTED_LIGHTS_LENGTH; i++)
  {
    CLpxCommand foo;
    foo.strand_index = i;
    foo.mode = ELpxModes::Waterfall;
    foo.delayMs = 50;

    //LpxRunner.runCommand(foo);

    foo.mode = ELpxModes::Off;

    //LpxRunner.runCommand(foo);
  }

  //fill_solid(CONNECTED_LIGHTS[0].strand, CONNECTED_LIGHTS[0].strand_length, CRGB(255, 0, 0));
  //FastLED[0].showLeds();

  Serial.println("Node_" + (String)LpxConfig.LPX_ID + "_Lit");

  //the loop to mimic the void loop()  @ core 0
  while (true)
  {
    if (xSemaphoreTake(binsem, 500 * portTICK_RATE_MS) == pdTRUE)
    {
      localCommands = globalCommands;
      xSemaphoreGive(binsem);
    }
    else
    {
      Serial.println("Failed to read semaphore");
    }

    for (byte i = 0; i < localCommands.size(); i++)
    {
      // LpxModes.solid(LpxConfig.CONNECTED_LIGHTS[localCommands[i].strand_index], localCommands[i].primary[0], localCommands[i].primary[1], localCommands[i].primary[2]);

      // for each command in the localcommand we run the
    }

    for (byte i = 0; i < LpxConfig.CONNECTED_PERIPHERALS_LENGTH; i++)
    {
      //for each strand we call the code
      LpxConfig.CONNECTED_LIGHTS[i].showStrand();
    }

    delay(1); //not using vTaskDelay(1 / portTICK_PERIOD_MS)
  }
}