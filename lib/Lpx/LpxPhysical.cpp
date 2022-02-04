#include "LpxPhysical.h"

//NOTE: we make a new task for each strand
void stripTaskHandler(void *instancePointer)
{
    //get the object that made this task & make a pointer to the stuff we need
    CLpxStrip *obj = (CLpxStrip *)instancePointer;
    bool oneTime = true;
    bool handler = obj->taskStateControl;

    while (true)
    {
        for (int i = 0; i < obj->commandList.size(); i++)
        {
            //store the current command
            CLpxCommand c = obj->commandList.at(0);

            //run the switch
            switch (c.mode)
            {
            case ELpxModes::Off:
                oneTime = LpxModes.off(obj, oneTime);
                break;
            case ELpxModes::Solid:
                oneTime = LpxModes.solid(obj, c, oneTime);
                break;
            case ELpxModes::RandomCloudy:
                LpxModes.randomCloudyBlobs(obj, c, oneTime);
                break;
            case ELpxModes::Flash:
                LpxModes.flash(obj, c, handler);
                break;
            case ELpxModes::Sweep:
                //empty bc it is gone
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
                LpxModes.rain(obj, c, handler);
                break;
            case ELpxModes::Pause:
                //empty bc that how it actually pauses
                break;
            case ELpxModes::SoundSync:
                //empty bc its gone
                break;
            case ELpxModes::RainbowWaterfall:
                oneTime = LpxModes.waterfallRainbow(obj, c, oneTime, handler);
                break;
            default:
                break;
            }

            //remove command from cue
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

//NOTE: declaring values for the CLpxStrip
int CLpxStrip::gIndex = 0;

CLpxStrip::CLpxStrip(int p, int l)
{
    index = gIndex;
    gIndex++;

    pin = p;
    strand_length = l;
    strand = new CRGB[l];

    taskStateControl = false;

    //okay so lets try making a new task that has the sole job of running this command
    xTaskCreatePinnedToCore(
        stripTaskHandler,
        "StripTask" + index,
        8000,
        (void *)this,
        1,
        &taskHandle,
        1);
}

void CLpxStrip::showStrand()
{
    // FastLED.show();
    FastLED[index].showLeds();

    // String y = "shown " + (String)millis();
    // Serial.println(y);

    // delay (10);
}

void CLpxStrip::commandAsync(CLpxCommand command)
{
    vTaskSuspend(taskHandle);

    //set out command
    commandList.clear();
    commandList.push_back(command);

    taskStateControl = !taskStateControl;

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

    taskStateControl = !taskStateControl;

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