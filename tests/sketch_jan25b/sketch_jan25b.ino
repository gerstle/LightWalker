#include <SPI.h>
#include <TCL.h>

const int STR_LENGTH = 25;
const int STR_COUNT = 2;
const int PIXEL_COUNT = STR_LENGTH * STR_COUNT;

const int LIGHT_STATUS_INIT = -1;
const int LIGHT_STATUS_OFF = 0;
const int LIGHT_STATUS_RED = 1;
const int LIGHT_STATUS_ROTATE = 2;

int light_status = LIGHT_STATUS_INIT;
int signaled_light_status = LIGHT_STATUS_INIT;

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
    colorpattern[2][0] = 0xff;
    colorpattern[2][1] = 0x00;
    colorpattern[2][2] = 0xff;

    TCL.begin();
    TCL.setupDeveloperShield();
    TCL.sendEmptyFrame();
    no_lights();
}

/* The loop is set up in two parts. Firs the Arduino does the work it needs to
 * do for every loop, next is runs the checkInput() routine to check and act on
 * any input from the serial connection.
 */
void loop()
{
    switch (signaled_light_status)
    {
        case LIGHT_STATUS_INIT:
            rotate_lights();
        case LIGHT_STATUS_RED:
            red();
            break;
        case LIGHT_STATUS_ROTATE:
            rotate_lights();
            break;
        case LIGHT_STATUS_OFF:
            no_lights();
            break;
    }

    // Accept and parse input
    checkInput();
}

void no_lights()
{
    TCL.sendEmptyFrame(); // <gerstle> rotate doesn't end on empty
    // <gerstle> blank out everything
    for (int i = 0; i < (STR_LENGTH * STR_COUNT); i++)
    {
        TCL.sendColor(0x00,0x00,0x00);
    }
    TCL.sendEmptyFrame();
}

void off()
{
    if (signaled_light_status != light_status)
    {
        Serial.println("goodnight");
        no_lights();
        light_status = LIGHT_STATUS_OFF;
    }
}

void red()
{
    if (signaled_light_status != light_status)
    {
        Serial.println("going red");
        light_status = LIGHT_STATUS_RED;
    }

    TCL.sendEmptyFrame();
    // <gerstle> blank out everything
    for (int i = 0; i < (STR_LENGTH * STR_COUNT); i++)
        TCL.sendColor(analogRead(TCL_POT4)>>2,0x00,0x00);
    TCL.sendEmptyFrame();
}

void rotate_lights()
{
    if (signaled_light_status != light_status)
    {
        Serial.println("rotating");
        no_lights();
        TCL.sendEmptyFrame();
        pixel_index = 0;
        pattern_index = 0;
    }

    currentMillis = millis();
    if ((currentMillis - previousMillis) > interval)
    {
        previousMillis = currentMillis;

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
    light_status = LIGHT_STATUS_ROTATE;
}

/* This routine checks for any input waiting on the serial line. If any is
 * available it is read in and added to a 128 character buffer. It sends back
 * an error should the buffer overflow, and starts overwriting the buffer
 * at that point. It only reads one character per call. If it receives a
 * newline character is then runs the parseAndExecuteCommand() routine.
 */
void checkInput()
{
    if (digitalRead(TCL_SWITCH2) == LOW)
        signaled_light_status = LIGHT_STATUS_OFF;
    else if (digitalRead(TCL_SWITCH1) == LOW)
        signaled_light_status = LIGHT_STATUS_RED;
    else
        signaled_light_status = LIGHT_STATUS_ROTATE;
}
