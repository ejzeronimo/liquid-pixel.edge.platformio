#include "Config.h"

CLpxConfig LpxConfig;

//NOTE: defining the contants now
const char *CLpxConfig::LPX_VERSION = "02.03.2022";

//DEBUG: ROBBIE CHANGE THIS TO THE ID ON THE BLUE TAPE
const char *CLpxConfig::LPX_ID = "E0002";

const char *CLpxConfig::SSID_NAME = "Noah";
const char *CLpxConfig::SSID_PASSPHRASE = "Abr@h@mFGsltw!3161463";

// const char *CLpxConfig::SSID_NAME = "lpx_wifi";
// const char *CLpxConfig::SSID_PASSPHRASE = "L1QU1DP1X3L";

const char *CLpxConfig::TARGET_IP = "172.16.17.57";
const int CLpxConfig::TARGET_PORT = 742;
String CLpxConfig::TARGET_ID = "";

//NOTE: define all the io under this

//DEBUG: ROBBIE UNCOMMENT THIS ONE IF IT IS AN EEL
#define i1 14
const int CLpxConfig::CONNECTED_PERIPHERALS_LENGTH = 1;
CLpxIO CLpxConfig::CONNECTED_PERIPHERALS[CONNECTED_PERIPHERALS_LENGTH] = {
    {i1,
     EPeripheralMode::Output,
     EPeripheralType::Digital}
};

//DEBUG: ROBBIE UNCOMMENT THIS ONE IF IT IS URSALA
// const int CLpxConfig::CONNECTED_PERIPHERALS_LENGTH = 0;
// CLpxIO CLpxConfig::CONNECTED_PERIPHERALS[CONNECTED_PERIPHERALS_LENGTH] = {};

//NOTE: define all the strands under this
//DEBUG: ROBBIE UNCOMMENT THIS ONE IF IT IS URSALA
// #define s1 14
// #define s2 27
// const int CLpxConfig::CONNECTED_LIGHTS_LENGTH = 2;
// CLpxStrip CLpxConfig::CONNECTED_LIGHTS[CONNECTED_LIGHTS_LENGTH] = {
//     {s1, 50},
//     {s2, 50}
// };

//DEBUG: ROBBIE UNCOMMENT THIS ONE IF IT IS URSALA
// #define s1 14
// const int CLpxConfig::CONNECTED_LIGHTS_LENGTH = 1;
// CLpxStrip CLpxConfig::CONNECTED_LIGHTS[CONNECTED_LIGHTS_LENGTH] = {
//     {s1, 55}
// };

const int CLpxConfig::CONNECTED_LIGHTS_LENGTH = 0;
CLpxStrip CLpxConfig::CONNECTED_LIGHTS[CONNECTED_LIGHTS_LENGTH] = {};


const void CLpxConfig::initConfig()
{
    //in here be sure to addleds() for any new strips

    //DEBUG: ROBBIE UNCOMMENT THIS ONE IF IT IS URSALA
    // FastLED.addLeds<WS2811, s1>(CONNECTED_LIGHTS[0].strand, CONNECTED_LIGHTS[0].strand_length).setCorrection(TypicalLEDStrip);
    // FastLED.addLeds<WS2811, s2>(CONNECTED_LIGHTS[1].strand, CONNECTED_LIGHTS[1].strand_length).setCorrection(TypicalLEDStrip);

    //DEBUG: ROBBIE UNCOMMENT THIS ONE IF IT IS AN EEL
    //FastLED.addLeds<WS2812, s1, GRB>(CONNECTED_LIGHTS[0].strand, CONNECTED_LIGHTS[0].strand_length).setCorrection(TypicalLEDStrip);

    //in here be sure to set pinmode
    //pulldown to ground if intput should be high

    //DEBUG: ROBBIE UNCOMMENT THIS ONE IF IT IS AN EEL
    pinMode(i1, OUTPUT);
}