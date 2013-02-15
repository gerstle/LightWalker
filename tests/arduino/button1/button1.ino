// <gerstle> simply reads button input and writes to stdout

int buttonPIN = 8;

void setup()
{
    Serial.begin(9600);
    pinMode(buttonPIN, INPUT);
}

void loop()
{
    if (digitalRead(buttonPIN) == HIGH)
        Serial.println("1"); 
    else
        Serial.println("0");
}
