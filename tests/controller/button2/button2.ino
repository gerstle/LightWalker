// <gerstle>
// 1 button, when pressed, flashes arduino led and 
// increments a pwm LED to one level brighter.
// after full brightness, the pwm LED turns off
//
// uses a debounce function on the button

int buttonPin = 8;
int ledPin = 13;
bool ledOn = false;

int dimLedPin = 11;
int dimLedLevel = 0;

bool currentButton = LOW;
bool lastButton = LOW;

void setup()
{
    Serial.begin(9600);
    pinMode(buttonPin, INPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(dimLedPin, OUTPUT);
}

void loop()
{
    //Serial.println(ledOn); 
    int buttonState = debounce(lastButton);

    if ((buttonState == HIGH) && !ledOn)
    {
        digitalWrite(ledPin, HIGH);
        ledOn = true;
        lastButton = HIGH;
        dimLedLevel = dimLedLevel + 51;

        if (dimLedLevel > 255)
            dimLedLevel = 0;

        //Serial.println(dimLedLevel);
        analogWrite(dimLedPin, dimLedLevel);
    }
    else if ((buttonState == LOW) && ledOn)
    {
        digitalWrite(ledPin, LOW);
        ledOn = false;
        lastButton = buttonState;
    }
}

bool debounce(bool lastState)
{
    bool currentState = digitalRead(buttonPin);

    if (lastState != currentState)
    {
        delay(5);
        currentState = digitalRead(buttonPin);
    }

    return currentState;
}
