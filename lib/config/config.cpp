#include "Config.h"

CLpxConfig LpxConfig;

//NOTE: defining the contants now
const char *CLpxConfig::LPX_VERSION = "01.01.2022";
const char *CLpxConfig::LPX_ID = "E0001";

const char *CLpxConfig::SSID_NAME = "WIFI";
const char *CLpxConfig::SSID_PASSPHRASE = "PASSWORD";

const char *CLpxConfig::TARGET_IP = "172.16.17.57";
const int CLpxConfig::TARGET_PORT = 742;

//NOTE: define all the io under this
#define i1 31
const int CLpxConfig::CONNECTED_PERIPHERALS_LENGTH = 1;
CLpxIO CLpxConfig::CONNECTED_PERIPHERALS[CONNECTED_PERIPHERALS_LENGTH] = {
    {i1,
     EPeripheralMode::Input,
     EPeripheralType::Digital}};

//NOTE: define all the strands under this
#define s1 14
#define s2 27
const int CLpxConfig::CONNECTED_LIGHTS_LENGTH = 2;
CLpxStrip CLpxConfig::CONNECTED_LIGHTS[CONNECTED_LIGHTS_LENGTH] = {
    {s1, 34},
    {s2, 46}};

const void CLpxConfig::initConfig()
{
    //in here be sure to addleds() for any new strips
    FastLED.addLeds<WS2811, s1>(CONNECTED_LIGHTS[0].strand, CONNECTED_LIGHTS[0].strand_length).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<WS2811, s2>(CONNECTED_LIGHTS[1].strand, CONNECTED_LIGHTS[1].strand_length).setCorrection(TypicalLEDStrip);

    //NEOPIXEL

    //in here be sure to set pinmode
    pinMode(i1, INPUT);
}