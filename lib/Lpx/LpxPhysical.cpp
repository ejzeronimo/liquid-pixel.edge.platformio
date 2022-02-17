#include "LpxPhysical.h"

// NOTE: declaring values for the CLpxStrip
int CLpxStrip::gIndex = 0;
int CLpxIO::gIndex = 0;

CLpxStrip::CLpxStrip(int p, int l) {
    index = gIndex;
    gIndex++;

    pin = p;
    strand_length = l;
    strand = new CRGB[l];

    brightness = 255;

    oneTimePerStrand = false;
}

void CLpxStrip::showStrand() {
    // we need to make this
    FastLED[index].showLeds(this->brightness);
}

void CLpxStrip::commandAsync(CLpxCommand command) {
    vTaskSuspend(this->taskHandle);

    // set out command
    this->commandList.clear();
    this->commandList.push_back(command);

    this->oneTimePerStrand = false;

    // String y = "restarting task " + (String)millis();
    // Serial.println(y);

    vTaskResume(this->taskHandle);
}

void CLpxStrip::commandAsync(std::vector<CLpxCommand> cmds) {
    vTaskSuspend(this->taskHandle);

    // make a tuple with this object and an array of commands
    this->commandList.clear();
    this->commandList = cmds;

    this->oneTimePerStrand = false;

    vTaskResume(this->taskHandle);
}

// NOTE: declaring values for the CLpxIO
CLpxIO::CLpxIO(int p, EPeripheralMode m, EPeripheralType t) {
    index = gIndex;
    gIndex++;

    pin = p;
    mode = m;
    type = t;

    eventTrigger = false;
    localEventValue = 0;
    localEventTriggered = false;
}

bool CLpxIO::setEvent(ELpxEventTypes e) {
    if (type == EPeripheralType::Analog && e != ELpxEventTypes::analog) {
        return false;
    }
    if (type == EPeripheralType::Digital && e == ELpxEventTypes::analog) {
        return false;
    }

    event = e;
    return true;
}

void CLpxIO::commandAsync(CLpxCommand command) {
    vTaskSuspend(this->taskHandle);

    // set out command
    this->commandList.clear();
    this->commandList.push_back(command);

    // String y = "restarting task " + (String)millis();
    // Serial.println(y);
    this->eventTrigger = false;

    vTaskResume(this->taskHandle);
}

void CLpxIO::commandAsync(std::vector<CLpxCommand> cmds) {
    vTaskSuspend(this->taskHandle);

    // make a tuple with this object and an array of commands
    this->commandList.clear();
    this->commandList = cmds;

    this->eventTrigger = false;

    vTaskResume(this->taskHandle);
}