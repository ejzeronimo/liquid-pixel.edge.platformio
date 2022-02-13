#include "LpxPhysical.h"

//NOTE: declaring values for the CLpxStrip
int CLpxStrip::gIndex = 0;

CLpxStrip::CLpxStrip(int p, int l)
{
    index = gIndex;
    gIndex++;

    pin = p;
    strand_length = l;
    strand = new CRGB[l];

    oneTimePerStrand = false;
}

void CLpxStrip::showStrand()
{
    //we need to make this 
    FastLED[index].showLeds(this->brightness);
}

void CLpxStrip::commandAsync(CLpxCommand command)
{
    vTaskSuspend(taskHandle);

    //set out command
    commandList.clear();
    commandList.push_back(command);

    // String y = "restarting task " + (String)millis();
    // Serial.println(y);

    vTaskResume(taskHandle);
}

void CLpxStrip::commandAsync(std::vector<CLpxCommand> cmds)
{
    vTaskSuspend(taskHandle);

    //make a tuple with this object and an array of commands
    commandList.clear();
    commandList = cmds;

    vTaskResume(taskHandle);
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