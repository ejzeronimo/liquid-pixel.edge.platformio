#include "LpxPhysical.h"

int CLpxStrip::gIndex = 0;

//the task that will be ran for each physical strand
void runStripHelper(void *pvParameters)
{
    // CLpxCommand command = *((CLpxCommand *)pvParameters);
    // CLpxStrip strip = LpxConfig.CONNECTED_LIGHTS[command.strand_index];

    while (true)
    {
        Serial.println("here");
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

CLpxStrip::CLpxStrip(int p, int l)
{
    index = gIndex;
    gIndex++;

    pin = p;
    strand_length = l;
    strand = new CRGB[l];

    //make a new thread in here
    //we make a new task
    xTaskCreatePinnedToCore(
        runStripHelper,
        "StripTask" + index,
        8000,
        NULL,
        10,
        &taskHandle,
        1);
}

void CLpxStrip::showStrand()
{
    FastLED[index].showLeds(255);
}

CLpxIO::CLpxIO(int p, EPeripheralMode m, EPeripheralType t)
{
    pin = p;
    mode = m;
    type = t;
}

bool CLpxIO::setEvent(ELpxEventTypes e)
{
    if (type == EPeripheralType::Analog && e != ELpxEventTypes::analog)
    {
        return false;
    }
    event = e;
    return true;
}