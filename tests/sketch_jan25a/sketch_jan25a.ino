#include <SPI.h>
#include <TCL.h>

const int STR_LENGTH = 25;
const int STR_COUNT = 2;
const int PIXEL_COUNT = STR_LENGTH * STR_COUNT;

unsigned long currentMillis;
long previousMillis = 0;
long interval = 30;

const int PATTERN_COUNT = 3;
int pattern_index = 0;
int pixel_index = 0;

byte colorpattern[PATTERN_COUNT][3];

void setup()
{
    Serial.begin(115200);

    // <gerstle> green
    colorpattern[0][0] = 0x00;
    colorpattern[0][1] = 0xff;
    colorpattern[0][2] = 0x00;
    // <gerstle> blue
    colorpattern[1][0] = 0x00;
    colorpattern[1][1] = 0x00;
    colorpattern[1][2] = 0xff;
    // <gerstle> purple
    colorpattern[2][0] = 0x80;
    colorpattern[2][1] = 0x00;
    colorpattern[2][2] = 0x80;

    TCL.begin();
    TCL.sendEmptyFrame();

    // <gerstle> blank out everything
    for (int i = 0; i < (STR_LENGTH * STR_COUNT); i++)
    {
        TCL.sendColor(0x00,0x00,0x00);
    }
    TCL.sendEmptyFrame();
}

void loop()
{
    String message = "top: pattern index: ";
    message = message + pattern_index + " pixel index: " + pixel_index;
    Serial.println(message);

    currentMillis = millis();
    if ((currentMillis - previousMillis) > interval)
    {
        Serial.println("--------------------> setting light");
        previousMillis = currentMillis;
        set_light();
    }
    
    Serial.println("bottom");
}

void set_light()
{
    if (pixel_index >= PIXEL_COUNT)
    {
        TCL.sendEmptyFrame();
        pixel_index = 0;
        pattern_index++;

        if (pattern_index >= PATTERN_COUNT)
            pattern_index = 0;
    }

    TCL.sendColor(colorpattern[pattern_index][0], colorpattern[pattern_index][1], colorpattern[pattern_index][2]);
    pixel_index++;
}
