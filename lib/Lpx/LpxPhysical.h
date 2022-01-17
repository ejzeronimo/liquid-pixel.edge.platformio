#pragma once
#include <FastLED.h>
#include <vector>
#include <tuple>

#include "LpxCommand.h"
#include "LpxModes.h"

class FastLED;
class CLpxCommand;
struct CRGB;

//NOTE: this is for the basic objects that will hold the physical strips and physical io information
class CLpxStrip
{
public:
    static int gIndex;

    TaskHandle_t taskHandle;
    CLpxCommand currentCommand;
    std::vector<CLpxCommand> currentQ;

    int index;
    int pin;
    int strand_length;
    CRGB *strand;

    //constructor
    //takes a pin and a length
    CLpxStrip(int p, int l);

    void showStrand();

    //starts and kills tasks instantly (note that some commands kill their task themselves)
    void commandAsync(CLpxCommand command);

    //same thing as single command but after execustion it moves onto the next command
    void commandAsyncQ(std::vector<CLpxCommand> commands);

    //will send a notif to a command that its time to end, then on exit the task will kill itself
    void commandSync(CLpxCommand command);

private:
    //creates a task while ensuring that the other associated task has been destroyed
    void startUniqueTask(TaskFunction_t loop, void *passthrough);

    //checkts he pointer and returns whether or not a task exists
    //true if running
    //false if dead
    bool getTaskStatus();
};

enum EPeripheralType : uint8_t
{
    Analog = 0,
    Digital
};

enum EPeripheralMode : int
{
    Input = 0,
    Input_Pullup,
    Input_Pulldown,
    Output
};

enum ELpxEventTypes : byte
{
    moment = 0x00,
    toggle = 0x01,
    hold = 0x02,
    analog = 0x03,
    unset = 0x04
};

class CLpxIO
{
public:
    int pin;
    EPeripheralType type;
    EPeripheralMode mode;
    ELpxEventTypes event = ELpxEventTypes::unset;

    bool eventTrigger;
    int localEventValue;
    bool localEventTriggered;

    CLpxIO(int p, EPeripheralMode m, EPeripheralType t);

    bool setEvent(ELpxEventTypes e);
};
