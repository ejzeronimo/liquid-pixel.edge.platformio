//these basics are needed until I swap over fully
#include <Arduino.h>
//these are network realted
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#include <vector>
//these are mine config last so all the vars are okay
#include <Lpx.h>
#include <Config.h>
//these are to disable brownout
#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>

//okay we need 2 threads
TaskHandle_t Task0;
TaskHandle_t Task1;

void Task0code(void *pvParameters);
void Task1code(void *pvParameters);

static SemaphoreHandle_t binsem;
static std::vector<CLpxCommand> globalCommands;

DynamicJsonDocument headRequest(512);

websockets::WebsocketsClient *client = new websockets::WebsocketsClient();

void setup()
{
  //this wraps out config, run here not in thread beacuse it might be needed
  LpxConfig.initConfig();

  binsem = xSemaphoreCreateBinary();
  xSemaphoreGive(binsem);

  setCpuFrequencyMhz(240);

  //create a task that will be executed in the Task0code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
      Task0code,   /* Task function. */
      "MainTask0", /* name of task. */
      10000,       /* Stack size of task */
      NULL,        /* parameter of the task */
      0,           /* priority of the task */
      &Task0,      /* Task handle to keep track of created task */
      0);          /* pin task to core 0 */
}

//kill the vanilla loop
void loop()
{
  vTaskDelete(NULL);
  return;
}

//NOTE: defining some of the function used only in the IOT loop here
void invokeLocalCommand(JsonObject header, JsonArray commands)
{
  for (int i = 0; i < commands.size(); i++)
  {
    // String y = "starting command " + millis();
    // Serial.println(y);

    //for each command set the right value
    CLpxCommand temp = LpxJson.handleCommandJson(commands[i], LpxConfig);
    LpxConfig.CONNECTED_LIGHTS[temp.strand_index].commandAsync(temp);
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
  {
    JsonArray obj = headRequest["deployment"]["peripherals"];
    JsonArray *ptr = &obj;

    //create a task that will be executed in the Task1code() function, with priority 0 and executed on core 1
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

    //make our header
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

  for (byte i = 0; i < LpxConfig.CONNECTED_LIGHTS_LENGTH; i++)
  {
    // CLpxCommand waterfallRainbow;
    // waterfallRainbow.delayMs = 50;
    // waterfallRainbow.mode = ELpxModes::Waterfall;
    // // waterfallRainbow.mode = ELpxModes::Solid;
    // // waterfallRainbow.primary[0] = 255;
    // // waterfallRainbow.primary[1] = 255;
    // // waterfallRainbow.primary[2] = 255;

    CLpxCommand off;
    off.mode = ELpxModes::Off;

    // CLpxCommand src[] = {waterfallRainbow, off};

    CLpxCommand src[] = { off};

    std::vector<CLpxCommand> dest;
    dest.insert(dest.begin(), std::begin(src), std::end(src));

    LpxConfig.CONNECTED_LIGHTS[i].commandAsync(dest);
  }

  Serial.println("Node_" + (String)LpxConfig.LPX_ID + "_Lit");

  //disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  WiFi.mode(WIFI_STA);
  WiFi.begin(LpxConfig.SSID_NAME, LpxConfig.SSID_PASSPHRASE);

  //renanble brownout detector
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

  //the loop to mimic the void loop() @ core 0
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

//NOTE: in here we should take care of all event triggers for IO
void Task1code(void *pvParameters)
{
  //take the jsonarray in
  JsonArray arr = *((JsonArray *)pvParameters);

  //we iterate through the array and config the IO
  for (int i = 0; i < arr.size(); i++)
  {
    JsonObject obj = arr[i];
    int index = obj["peripheral_index"];

    LpxConfig.CONNECTED_PERIPHERALS[index].mode = obj["type"];
  }

  Serial.println("Node_" + (String)LpxConfig.LPX_ID + "_Events_Online");

  //the loop to mimic the void loop() @ core 1
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
        //TODO: make analog work (its really noisy)
        // int temp = analogRead(io->pin);
        // if (temp != io->localEventValue)
        // {
        //   io->localEventValue = temp;
        // }
      }
      break;
      default:
        //its unset
        break;
      }

      //takes care of the data event
      if (io->eventTrigger)
      {
        DynamicJsonDocument response(512);

        //make our header
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
          //Serial.print("moment");
          io->eventTrigger = false;
        }
        break;
        case ELpxEventTypes::toggle:
        {
          //Serial.print("toggle");
          //Serial.print(io->localEventTriggered);
          returnObject["state"] = io->localEventTriggered;

          io->eventTrigger = false;
        }
        break;
        case ELpxEventTypes::hold:
        {
          if (io->localEventTriggered)
          {
            //Serial.print("holddown");
            returnObject["state"] = "down";
          }
          else
          {
            returnObject["state"] = "up";
            //Serial.print("holdrelease");
          }

          io->eventTrigger = false;
        }
        break;
        case ELpxEventTypes::analog:
        {
          //TODO:
        }
        break;
        default:
          //its unset
          break;
        }
        response["request"]["events"][0] = returnDoc;
        returnDoc.clear();

        String output;
        serializeJson(response, output);
        //Serial.println(output);

        //suspend out main task so we can send (maybe)
        //vTaskSuspend(Task0);
        client->send(output);
        //vTaskResume(Task0);

        response.clear();
      }
    }
    vTaskDelay(1 / portTICK_RATE_MS);
    // delayMicroseconds(100);
  }
}