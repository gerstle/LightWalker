// <gerstle> lightwalker.ino
//
// sketch for the LightWalker costume
// The main work is in the LW & Leg classes

// fix Arduino IDE regex issue with detecting first non-preprocessor directive
// (breaks when #ifdef is false and contains non-preprocessor line, see:
// http://code.google.com/p/arduino/issues/detail?id=156)

#ifdef __MK20DX256__
    #include <i2c_t3.h>
#else
    // <gerstle> comment out for teensy!
    // something is ignoring the ifdef's and including wire regardless which throws multiple definition
    // errors
    //#include <Wire.h>
#endif

#include "FastLED.h"
#include "ADXL345_compatible.h"
#include "LW.h"
#include "audio.h"

LW lw;

// <gerstle> bluetooth comms
const int msgLength = 128;
char msg[msgLength];
int msgIndex = 0;
char *pKey = NULL;
char *pValue = NULL;
char one_str[] = "1";

void setup()
{
    delay(3000);
    Serial.println(". ");
    // <gerstle> general setup
    Serial.begin(9600);

    // <gerstle> bluetooth setup
    Serial.print("bluetooth... ");

    // <gerstle> need to fix my bluetooth so they're all one baud rate
#ifdef __MK20DX256__
    Serial1.begin(9600);
#else
    Serial1.begin(57600);
#endif

    delay(200);
    Serial1.print("SettingsPlease\r");
    Serial.println("check");

    // <gerstle> lights setup
    Serial.print("leds... ");
    LEDS.addLeds<P9813, LED_CLOCK_PIN, LED_DATA_PIN, RGB, DATA_RATE_MHZ(15)>((CRGB *)(lw.leds), LED_COUNT);
    LEDS.setBrightness(50);
    LEDS.showColor(CRGB::Green);
    delay(400);
    Serial.println("check");

    // <gerstle> audio setup
    Serial.print("microphone... ");
    pinMode(AUDIO_PIN, INPUT);
    audioSetup();
    Serial.println("check");

    // <cgerstle> Join i2c bus as master
    Serial.print("i2c bus...");
    Wire.begin();
    Serial.println("check");

    Serial.println("legs...");
    lw.initLegs(masterOff);
    Serial.println("    check");
    Serial.println("Walking!");
}

unsigned long statusTimer;
unsigned long currentTime;
int frameCounter = 0;
void loop()
{
    // <gerstle> perform LightWalker action
    lw.walk();

    currentTime = millis();
    frameCounter++;
    if (currentTime >= (statusTimer + 1000))
    {
        statusTimer = currentTime;
        Serial.print("["); Serial.print(frameCounter); Serial.println("]");
        frameCounter = 0;
    }

    // <gerstle> get commands from bluetooth... should switch this to
    // be an interrupt again...
    if (Serial1.available())
    {
        msg[msgIndex++] = (char)Serial1.read();

        if (msg[msgIndex - 1] == '\r')
        {
            pValue = strchr(msg, '=');
            if (pValue != NULL)
            {
                pKey = msg;
                *pValue = '\0';
                pValue++;

                if (executeCommand(atoi(pKey), pValue, msgIndex - (pValue - pKey) - 1))
                    Serial1.print("OK\r");
                else
                    Serial1.print("?\r");
            }

            memset(msg, '\0', msgIndex - 1);
            msgIndex = 0;
        }
    }
}

bool executeCommand(int key, char *value, int valueLen)
{
    Serial.print("key: "); Serial.print(key);
    Serial.print(" value: "); Serial.print(value); Serial.print("("); Serial.print(valueLen); Serial.print(")");

    int valueInt;

    switch (key)
    {
        // ------------------------------------------------------------------------
        // Main 
        // ------------------------------------------------------------------------
        case mainMaxBrightness:
            lw.config.main.maxBrightness = atoi(value);
            LEDS.setBrightness(lw.config.main.maxBrightness);
            break;
        case mainDefaultMode:
            break;

        // ------------------------------------------------------------------------
        // Mode 
        // ------------------------------------------------------------------------
        case mode:
            lw.setMode(static_cast<WalkingModeEnum>(atoi(value)));
            break;

        // ------------------------------------------------------------------------
        // Sparkle
        // ------------------------------------------------------------------------
        case sparkleFadeRate:
            lw.config.sparkle.fadeRate = atoi(value);
            break;  
        case sparkleFlashLength:
            lw.config.sparkle.flashLength = atoi(value);
            break;
        case sparkleSparkleLength:
            lw.config.sparkle.sparkleLength = atoi(value);
            break;
        case sparkleFootFlashColor:
            ParseColor(value, &(lw.config.sparkle.footFlashColor));
            break;
        case sparkleSparkleColor:
            ParseColor(value, &(lw.config.sparkle.sparkleColor));
            break;
        case sparkleMinValue:
            lw.config.sparkle.minValue = atoi(value);
            break;

        // ------------------------------------------------------------------------
        // Pulse
        // ------------------------------------------------------------------------
        case pulseMinRate:
            lw.config.pulse.minPulseTime = atoi(value);
            break;
        case pulseMaxRate:
            lw.config.pulse.maxPulseTime = atoi(value);
            break;
        case pulseSyncLegs:
            if (strncmp(value, one_str, valueLen) == 0)
                lw.config.pulse.syncLegs = true;
            else
                lw.config.pulse.syncLegs  = false;
            break;
        case pulseColor:
            ParseColor(value, &(lw.config.pulse.color));
            LWUtils.printRGB(lw.config.pulse.color, true);
            break;
        case pulseMode:
            lw.config.pulse.mode = static_cast<PulseMode>(atoi(value));
            //lw.resetPulseMode();
            break;

        // ------------------------------------------------------------------------
        // Equalizer
        // ------------------------------------------------------------------------
        case eqMode:
            lw.config.equalizer.mode = static_cast<EqualizerMode>(atoi(value));
            break;
        case eqColor:
            ParseColor(value, &(lw.config.equalizer.color));
            break;
        case eqAllBands:
            if (strncmp(value, one_str, 1) == 0)
                lw.config.equalizer.allBands = true;
            else
                lw.config.equalizer.allBands = false;
            break;
        case eqMinValue:
            lw.config.equalizer.minValue = atoi(value);
            break;

        // ------------------------------------------------------------------------
        // Gravity
        // ------------------------------------------------------------------------
        case gravityRotate:
            if (strncmp(value, one_str, 1) == 0)
                lw.config.gravity.rotate = true;
            else
                lw.config.gravity.rotate = false;
            break;
        case gravityMinValue:
            lw.config.gravity.minValue= atoi(value);
            break;

        //------------------------------------------------------------------------
        // Bubble
        //------------------------------------------------------------------------
        case bubbleBackgroundColor:
            ParseColor(value, &(lw.config.bubble.backgroundColor));
            break;
        case bubbleBubbleColor:
            ParseColor(value, &(lw.config.bubble.bubbleColor));
            break;
        case bubbleSpeed:
            lw.config.bubble.speed = atoi(value);
            break;
        case bubbleWidth:
            lw.config.bubble.width = atoi(value);
            break;
        case bubbleTrail:
            if (strncmp(value, one_str, 1) == 0)
                lw.config.bubble.trail = true;
            else
                lw.config.bubble.trail = false;
            break;
    
        //------------------------------------------------------------------------
        // Rainbow
        //------------------------------------------------------------------------
        case rainbowMode:
            lw.config.rainbow.mode = static_cast<RainbowMode>(atoi(value));
            break;
        case rainbowMinValue:
            lw.config.rainbow.minValue= atoi(value);
            break;
        case rainbowDelay:
            lw.config.rainbow.delay = atoi(value);
            break;
        
        //------------------------------------------------------------------------
        // Zebra
        //------------------------------------------------------------------------
        case zebraColorOne:
            ParseColor(value, &(lw.config.zebra.colorOne));
            break;
        case zebraColorTwo:
            ParseColor(value, &(lw.config.zebra.colorTwo));
            break;
        
        //------------------------------------------------------------------------
        // Chaos
        //------------------------------------------------------------------------
        
        //------------------------------------------------------------------------
        // Flames
        //------------------------------------------------------------------------

        default:
            Serial.print(" <------------- not recognized");
            return false;
            break;
    }

    Serial.println();
    return true;
}

void ParseColor(char *colorString, CHSV *color)
{
    char *value = colorString;
    char *pComma = strchr(colorString, ',');
    *pComma = '\0';

    (*color).hue = byte(atoi(value));

    value = pComma + 1;
    pComma = strchr(value, ',');
    *pComma = '\0';

    (*color).sat = byte(atoi(value));
    (*color).val = byte(atoi(pComma + 1));
}
