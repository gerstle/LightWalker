// <gerstle> lightwalker.ino
//
// sketch for the LightWalker costume
// The main work is in the LW & Leg classes

#include <i2c_t3.h>
#include "ADXL345_t3.h"
#include "FastSPI_LED2.h"
#include "LW.h"

LW lw;

// <gerstle> bluetooth comms
HardwareSerial Uart = HardwareSerial();
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
    Uart.begin(57600);
    delay(200);
    Uart.print("SettingsPlease\r");
    Serial.println("check");

    // <gerstle> lights setup
    Serial.print("leds... ");
    LEDS.addLeds<P9813, 11, 13, BGR>((CRGB *)(lw.leds), LED_COUNT);
    LEDS.setBrightness(25);
    LEDS.showColor(CRGB::Green);
    delay(400);
    Serial.println("check");

    // <gerstle> audio setup
    Serial.print("microphone... ");
    pinMode(AUDIO_PIN, INPUT);
    analogReadResolution(ANALOG_READ_RESOLUTION);
    analogReadAveraging(ANALOG_READ_AVERAGING);
    Serial.println("check");

    // <cgerstle> Join i2c bus as master
    Serial.print("i2c bus...");
    Wire.begin();
    Serial.println("check");

    Serial.println("legs...");
    LEDS.showColor(CRGB::Black);
    lw.initLegs(masterOff);
    Serial.println("    check");
    Serial.println("Walking!");
}

elapsedMillis statusTimer;
int frameCounter = 0;
void loop()
{
    // <gerstle> perform LightWalker action
    lw.walk();

    frameCounter++;
    if (statusTimer >= 1000)
    {
        statusTimer = 0;
        Serial.print("["); Serial.print(frameCounter); Serial.println("]");
        frameCounter = 0;
    }

    // <gerstle> get commands from bluetooth... should switch this to
    // be an interrupt again...
    if (Uart.available())
    {
        msg[msgIndex++] = (char)Uart.read();

        if (msg[msgIndex - 1] == '\r')
        {
            pValue = strchr(msg, '=');
            if (pValue != NULL)
            {
                pKey = msg;
                *pValue = '\0';
                pValue++;

                if (executeCommand(atoi(pKey), pValue, msgIndex - (pValue - pKey) - 1))
                    Uart.print("OK\r");
                else
                    Uart.print("?\r");
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
        case eqColor:
            ParseColor(value, &(lw.config.equalizer.color));
            break;
        case eqRMSThreshold:
            lw.config.equalizer.rmsThreshold = atoi(value);
            break;
        case eqAllLights:
            if (strncmp(value, one_str, 1) == 0)
                lw.config.equalizer.allLights = true;
            else
                lw.config.equalizer.allLights = false;
            break;
        case eqAllBands:
            if (strncmp(value, one_str, 1) == 0)
                lw.config.equalizer.allBands = true;
            else
                lw.config.equalizer.allBands = false;
            break;
        case eqRainbow:
            if (strncmp(value, one_str, 1) == 0)
                lw.config.equalizer.rainbow = true;
            else
                lw.config.equalizer.rainbow = false;
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
            InitBubbleBackgroundColors();
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

// <cgerstle> the idea here is to cache 3 sparkle colors that can then be used
// instead of calculating during running of bubble
void InitBubbleBackgroundColors()
{
    float max = ((float) min(40, lw.config.main.maxBrightness)) / 100;
    float brightness = 0;

    for (int i = 0; i < 3; i++)
    {
        lw.config.bubble.backgroundColors[i] = CHSV(lw.config.bubble.backgroundColor.hue, lw.config.bubble.backgroundColor.saturation, (byte)max(0, lw.config.bubble.backgroundColor.value - random(0, 100)));
    }
}

