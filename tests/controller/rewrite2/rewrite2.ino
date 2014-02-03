#include <SoftwareSerial.h>
#include <Wire.h>
#include <ADXL345.h>
#include <SPI.h>
#include <TCL.h>

#define DEBUG true

// <gerstle> TX is the arduino's TX... attach to bluetooth RX
// <gerstle> RX is the arduino's RX... attach to bluetooth TX
#define TXPIN 10
#define RXPIN 9 

#define LEG_COUNT 2
#define PIXELS_PER_LEG 25

enum WalkingModeEnum
{
    Main,
    Gravity,
    Equalizer,
    Sparkle,
    Pulse
};

struct MainConfigs
{
    MainConfigs() : maxBrightness(200), defaultMode(Pulse) {}

    int maxBrightness;
    WalkingModeEnum defaultMode;
};

struct SparkleConfigs
{
    SparkleConfigs() : footFlashColor(COLORS[WHITE]), footSparkleColor(COLORS[WHITE]), legSparkleColor(COLORS[WHITE]), footDownFadeRate(0x4), footUpFadeRate(0x8), flashLength(500), sparkleLength(500) {}

    byte footFlashR;
    byte footFlashG;
    byte footFlashB;
    byte footSparkleR;
    byte footSparkleG;
    byte footSparkleB;
    byte legSparkleR;
    byte legSparkleG;
    byte legSparkleB;
    int flashLength;
    int sparkleLength;
    byte footDownFadeRate;
    byte footUpFadeRate;
};

struct PulseConfigs
{
    PulseConfigs() : color(COLORS[WHITE]), minBrightness(0), maxBrightness(200), minPulseTime(1000), maxPulseTime(5000), randomColor(false), syncLegs(false) {}

    byte r;
    byte g;
    byte b;
    int minBrightness;
    int maxBrightness;
    int minPulseTime;
    int maxPulseTime;
    bool randomColor;
    bool syncLegs;
};

PulseConfigs pulseConfigs;
SparkleConfigs sparkleConfigs;
MainConfigs mainConfigs;

Leg legs[LEG_COUNT] =
{
    Leg("left leg", 2, Pulse),
    Leg("right leg", 3, Pulse)
};
unsigned long _laststatus;
WalkingModeEnum _mode;

unsigned long _lightModeChangeTime;

// <gerstle> pulse
int _pulse_length;
bool _pulse_isDimming
SoftwareSerial bluetooth(RXPIN, TXPIN);

void setup()
{
    // <gerstle> general setup
    Serial.begin(9600);
    Serial.println("setting up...");
    randomSeed(long(millis()));

    // <gerstle> bluetooth setup
    pinMode(RXPIN, INPUT);
    pinMode(TXPIN, OUTPUT);
    bluetooth.begin(57600);

    // <gerstle> lights setup
    TCL.begin();
    TCL.setupDeveloperShield();
    TCL.sendEmptyFrame();

    // <gerstle> ask for initial settings for current mode
    setMode(legs, MasterOff);
    bluetooth.print("SettingsPlease\r");
    Serial.println("Walking!");
}

String msg = "";
void loop()
{
    // <cgerstle> get commands from bluetooth
    if (bluetooth.available())
    {
        msg = "";
        char lastRead = bluetooth.read();
        msg.concat(lastRead);

        // <gerstle> seems like the \r gets horked sometimes and that the message
        // sent has a funny y attached to the end. So, check that we're within
        // valid ascii. If not, consider that the end of the message
        while (lastRead != '\r')
        {
            lastRead = bluetooth.read();
            if ((lastRead > (char)0x20) && (lastRead < (char)0x7F))
                msg.concat(lastRead);
        }

        if (ExecuteCommand(String(msg)))
            bluetooth.print("OK\r");
    }

    // <gerstle> perform LightWalker action
    lw.walk(legs);
}
 
bool ExecuteCommand(String input)
{
    int equalsIndex = input.indexOf('=');

    if (equalsIndex <= 0)
        return false;

    String key = input.substring(0, equalsIndex);
    String value = input.substring(equalsIndex + 1);

    Serial.print("key: "); Serial.print(key); Serial.print(" value: "); Serial.println(value);

    // ------------------------------------------------------------------------
    // MAIN
    // ------------------------------------------------------------------------
    if (key == "mainPrefMaxBrightness")
    {
        mainConfigs.maxBrightness = value.toInt();
        Serial.print("max set to: "); Serial.println(mainConfigs.maxBrightness);
    }

    // ------------------------------------------------------------------------
    // Mode
    // ------------------------------------------------------------------------
    else if (key == "mode")
    {
        WalkingModeEnum mode;

        if (value == "gravity")
            mode = Gravity;
        else if (value == "equalizer")
            mode = Equalizer;
        else if (value == "sparkle")
            mode = Sparkle;
        else if (value == "pulse")
            mode = Pulse;
        else
            return false;

        //lw.setMode(mode);
    }

    // ------------------------------------------------------------------------
    // Gravity
    // ------------------------------------------------------------------------
//                 if (key == "x")
//                     ParseColor(value, &color); 
//                 else if (key == "y")
//                     ParseColor(value, &color); 
//                 else if (key == "z")
//                     ParseColor(value, &color); 

    // ------------------------------------------------------------------------
    // SPARKLE
    // ------------------------------------------------------------------------
    else if (key == "sparklePrefFootFlashColor")
    {
        sparkleConfigs.footFlashColor.r = ParseColor(value);
        sparkleConfigs.footFlashColor.g = ParseColor(value);
        sparkleConfigs.footFlashColor.b = ParseColor(value);
    }
    else if (key == "sparklePrefFootSparkleColor")
    {
        sparkleConfigs.footSparkleColor.r = ParseColor(value);
        sparkleConfigs.footSparkleColor.g = ParseColor(value);
        sparkleConfigs.footSparkleColor.b = ParseColor(value);
    }
    else if (key == "sparklePrefLegSparkleColor")
    {
        sparkleConfigs.legSparkleColor.r = ParseColor(value);
        sparkleConfigs.legSparkleColor.g = ParseColor(value);
        sparkleConfigs.legSparkleColor.b = ParseColor(value);
    }
    else if (key == "sparklePrefFlashLength")
        sparkleConfigs.flashLength = value.toInt();
    else if (key == "sparklePrefSparkleLength")
        sparkleConfigs.sparkleLength = value.toInt();
    else if (key == "sparklePrefFootDownFadeRate")
        sparkleConfigs.footDownFadeRate = byte(value.toInt());
    else if (key == "sparklePrefFootUpFadeRate")
        sparkleConfigs.footUpFadeRate = byte(value.toInt());

    // ------------------------------------------------------------------------
    // PULSE
    // ------------------------------------------------------------------------
    else if (key == "pulsePrefColor")
    {
        pulseConfigs.color.r = ParseColor(value);
        pulseConfigs.color.g = ParseColor(value);
        pulseConfigs.color.b = ParseColor(value);
    }
    else if (key == "pulsePrefMinRate")
        pulseConfigs.minPulseTime = value.toInt();
    else if (key == "pulsePrefMaxRate")
        pulseConfigs.maxPulseTime = value.toInt();
    else if (key == "pulsePrefRandomColor")
        if (value == "true")
            pulseConfigs.randomColor = true;
        else
            pulseConfigs.randomColor = false;
    else if (key == "pulsePrefSyncLegs")
        if (value == "true")
            pulseConfigs.syncLegs = true;
        else
            pulseConfigs.syncLegs = false;

    return true;
}

// <gerstle> just get the first integer off of the string and return it
byte ParseColor(String colorString)
{
    int commaIndex = colorString.indexOf(',');

    colorString = colorString.substring(commaIndex + 1);
    return byte(colorString.substring(0, commaIndex).toInt());
}

void setMode(WalkingModeEnum mode)
{
    _mode = mode;
    _lightModeChangeTime = millis();
    for (int i = 0; i < LEG_COUNT; i++)
        _legs[i].setWalkingMode(mode);

    if (DEBUG)
    {
        switch (mode)
        {
            case Pulse:
                Serial.println("pulse configs");
                Serial.print("    color: "); Serial.print(pulseConfigs.color.r); Serial.print(" - "); Serial.print(pulseConfigs.color.g); Serial.print(" - "); Serial.println(pulseConfigs.color.b);
                Serial.print("    min bright: "); Serial.println(pulseConfigs.minBrightness);
                Serial.print("    max bright: "); Serial.println(pulseConfigs.maxBrightness);
                Serial.print("    min pulse time: "); Serial.println(pulseConfigs.minPulseTime);
                Serial.print("    max pulse time: "); Serial.println(pulseConfigs.maxPulseTime);
                Serial.print("    random color: "); Serial.println(pulseConfigs.randomColor);
                Serial.print("    sync: "); Serial.println(pulseConfigs.syncLegs);
                break;
            case Gravity:
                break;
        }
    }
}

void walk()
{
    TCL.sendEmptyFrame();

    unsigned long currentTime = millis();
    bool displayStatus = false;
    bool pulseChangeColor = false;
    if (currentTime > (_laststatus + 2000))
        displayStatus = false;

    // <gerstle> foreach leg, check the sensor
    for (int i = 0; i < LEG_COUNT; i++)
    {
        bool sensorState = digitalRead(_legs[i].trigger_pin);
        
        if (displayStatus && DEBUG)
        {
            Serial.print("    leg "); Serial.println(_legs[i].name);
            Serial.print("        pin: "); Serial.println(_legs[i].trigger_pin);
            Serial.print("        pin state: "); Serial.println(sensorState);
            Serial.print("        status: "); Serial.println(_legs[i].status);
            _laststatus = millis();
        }

        switch (_mode)
        {
            case Sparkle:

                //if ((sensorState == LOW) && (_legs[i].status != Down))
                if ((sensorState == HIGH) && (_legs[i].status != Down))
                    _legs[i].sparkle_footdown();
                //else if ((sensorState == HIGH) && (_legs[i].status != Up))
                else if ((sensorState == LOW) && (_legs[i].status != Up))
                    _legs[i].sparkle_footup();
                else
                    _legs[i].sparkle_sameStatus();

                //if (
                    _legs[i].sparkle_fade_on = true;
                //else
                    //_legs[i].sparkle_fade_on = false;
                break;

            case Gravity:
                break;

            case Equalizer:
                break;

            case Pulse:
                if (pulseConfigs.syncLegs && (currentTime >= (_lightModeChangeTime + (_pulse_length * 2))))
                {
                    _pulse_length = random(pulseConfigs.minPulseTime, pulseConfigs.maxPulseTime);
                    _lightModeChangeTime = currentTime;
                    _pulse_isDimming = false;
                    pulseChangeColor = true;
                }
                else if (currentTime >= (_lightModeChangeTime + _pulse_length))
                    _pulse_isDimming = true;
                
                int value;
                if (pulseConfigs.syncLegs)
                    if (_pulse_isDimming)
                        value = (_pulse_length * 2) - (currentTime - _lightModeChangeTime);
                    else
                        value = currentTime - _lightModeChangeTime;

                _legs[i].pulse_pulse(currentTime, _lightModeChangeTime, _pulse_length, value, pulseChangeColor);
                break;
        }
    }
    TCL.sendEmptyFrame();
}
