#include "LpxPhysical.h"

//NOTE: declaring values for the CLpxStrip
int CLpxStrip::gIndex = 0;

//this one just runs a command and does not care
void singleCommandHelper(void *instancePointer)
{
    //get the object that made this task & cache
    CLpxStrip obj = *((CLpxStrip *)instancePointer);
    CLpxCommand com = obj.currentCommand;

    //define our run parameter
    bool runCycle = true;

    while (runCycle)
    {
        switch (com.mode)
        {
        case ELpxModes::Off:
            LpxModes.off(obj);
            runCycle = false;
            break;
        case ELpxModes::Solid:
            LpxModes.solid(obj, com);
            runCycle = false;
            break;
        case ELpxModes::Waterfall:
            LpxModes.waterfallRainbow(obj, com);
            runCycle = false;
            break;
        case ELpxModes::Flash:
            LpxModes.flash(obj, com);
            break;
        default:
            break;
        }

        vTaskDelay(1 / portTICK_RATE_MS);
    }

    //we kill once we exit our loop
    CLpxStrip *killObj = (CLpxStrip *)instancePointer;
    killObj->taskHandle = NULL;
    vTaskDelete(NULL);
}

void multipleCommandHelper(void *instancePointer)
{
    CLpxStrip obj = *((CLpxStrip *)instancePointer);
    std::vector<CLpxCommand> cmds = obj.currentQ;

    //define our run parameter
    int runCount = cmds.size();

    while (runCount != 0)
    {
        for (int i = 0; i < cmds.size(); i++)
        {

            switch (cmds.at(i).mode)
            {
            case ELpxModes::Off:
                LpxModes.off(obj);
                runCount--;
                break;
            case ELpxModes::Solid:
                LpxModes.solid(obj, cmds.at(i));
                runCount--;
                break;
            case ELpxModes::Waterfall:
                LpxModes.waterfallRainbow(obj, cmds.at(i));
                runCount--;
                break;
            default:
                break;
            }
        }

        delay(0);
    }

    Serial.println("kill");

    //we kill once we exit our loop
    CLpxStrip *killObj = (CLpxStrip *)instancePointer;
    killObj->taskHandle = NULL;
    vTaskDelete(NULL);
}

CLpxStrip::CLpxStrip(int p, int l)
{
    index = gIndex;
    gIndex++;

    pin = p;
    strand_length = l;
    strand = new CRGB[l];
}

void CLpxStrip::showStrand()
{
    FastLED[index].showLeds();
}

void CLpxStrip::commandAsync(CLpxCommand command)
{
    //set out command
    currentCommand = command;

    startUniqueTask(singleCommandHelper, (void *)this);
}

void CLpxStrip::commandAsyncQ(std::vector<CLpxCommand> cmds)
{
    //make a tuple with this object and an array of commands
    currentQ = cmds;

    startUniqueTask(multipleCommandHelper, (void *)this);
}

void CLpxStrip::commandSync(CLpxCommand command)
{
    //we need to wait here

    //set out command
    currentCommand = command;

    startUniqueTask(singleCommandHelper, (void *)this);

    //might not need
    while (getTaskStatus())
    {
        //spin our wheels
        delayMicroseconds(100);
    }
}

void CLpxStrip::startUniqueTask(TaskFunction_t l, void *v)
{
    //if our task handle is not null kill it with fire
    if (taskHandle != NULL)
    {
        vTaskDelete(taskHandle);
    }

    //okay so lets try making a new task that has the sole job of running this command
    xTaskCreatePinnedToCore(
        l,
        "StripTask" + index,
        8000,
        v,
        1,
        &taskHandle,
        1);
}

bool CLpxStrip::getTaskStatus()
{
    //if our task handle is not null then it exists
    if (taskHandle != NULL)
    {
        return true;
    }
    return false;
}

//NOTE: declaring values for the CLpxIO
CLpxIO::CLpxIO(int p, EPeripheralMode m, EPeripheralType t)
{
    pin = p;
    mode = m;
    type = t;

    eventTrigger = false;
    localEventValue = 0;
    localEventTriggered = false;
}

bool CLpxIO::setEvent(ELpxEventTypes e)
{
    if (type == EPeripheralType::Analog && e != ELpxEventTypes::analog)
    {
        return false;
    }
    if (type == EPeripheralType::Digital && e == ELpxEventTypes::analog)
    {
        return false;
    }

    event = e;
    return true;
}