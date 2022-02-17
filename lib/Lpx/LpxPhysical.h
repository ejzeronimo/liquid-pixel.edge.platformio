#pragma once
#include <FastLED.h>
#include <tuple>
#include <vector>

#include "LpxCommand.h"
#include "LpxModes.h"

class FastLED;
class CLpxCommand;
struct CRGB;

// NOTE: this is for the basic objects that will hold the physical strips and physical io information
class CLpxStrip {
public:
    // for construction
    static int gIndex;

    // for the task that each strand will have
    TaskHandle_t taskHandle;

    // stuff the modes will use
    std::vector<CLpxCommand> commandList;
    bool oneTimePerStrand;

    // mutable state that is unique to this strand
    int brightness;

    // immutable state for this trand
    CRGB *strand;
    int strand_length;
    int index;
    int pin;

    // constructor
    // takes a pin and a length
    CLpxStrip(int p, int l);

    void showStrand();

    // runs a single command (overload also uses same task handler just different vectorloading)
    void commandAsync(CLpxCommand command);

    // same thing as single command but after execution it moves onto the next command
    void commandAsync(std::vector<CLpxCommand> commands);
};

enum EPeripheralType : uint8_t {
    Analog = 0,
    Digital
};

enum EPeripheralMode : uint8_t {
    Input = 0x00,
    Input_Pullup,
    Input_Pulldown,
    Output,
};

enum ELpxEventTypes : uint8_t {
    moment = 0x00,
    toggle = 0x01,
    hold = 0x02,
    analog = 0x03,
    unset = 0x04
};

class CLpxIO {
public:
    // for construction
    static int gIndex;

    int pin;
    EPeripheralType type;
    EPeripheralMode mode;
    ELpxEventTypes event = ELpxEventTypes::unset;

    bool eventTrigger;
    int localEventValue;
    bool localEventTriggered;
    int index;

    // for the task that each io will have
    TaskHandle_t taskHandle;
    std::vector<CLpxCommand> commandList;

    CLpxIO(int p, EPeripheralMode m, EPeripheralType t);

    bool setEvent(ELpxEventTypes e);

    // runs a single command (overload also uses same task handler just different vectorloading)
    void commandAsync(CLpxCommand command);

    // same thing as single command but after execution it moves onto the next command
    void commandAsync(std::vector<CLpxCommand> commands);
};
