#include <SPI.h>
#include <TCL.h>

int soundPIN = 5;

const int valueCount = 8;
int values[valueCount];
int valueIndex = 0;
int valueTotal = 0;
int valueAvg = 0;

int pixel_count = 50;

void setup() 
{ 
    Serial.begin(9600);
    pinMode(soundPIN, INPUT);

    TCL.begin();
    TCL.setupDeveloperShield();
    TCL.sendEmptyFrame();

    for (valueIndex = 0; valueIndex < valueCount; valueIndex++)
    {
        values[valueIndex] = analogRead(soundPIN);
        valueTotal += values[valueIndex];
    }
    
    valueAvg = valueTotal / valueCount;
}

void loop() 
{ 
    int newValue = analogRead(soundPIN);

    if (valueIndex >= valueCount)
        valueIndex = 0;

    valueTotal = valueTotal - values[valueIndex] + newValue;
    values[valueIndex] = newValue;
    valueAvg = valueTotal / valueCount;

    int bars = valueAvg / 20;

    Serial.print(valueAvg); Serial.print("\t");
    float brightness = ((float)map(valueAvg, 56, 450, 0, 100)) / 100;
    Serial.println(brightness);
    int color = byte(((float) 255 * (brightness * brightness)));

    if (color > 255)
        color = 255;
    //Serial.println(valueAvg);

    TCL.sendEmptyFrame();
    for (int i = 0; i < pixel_count; i++)
        TCL.sendColor(color, color, color);

//     for (int i = 0; i < bars; i++)
//         Serial.print("--");

//     Serial.println(".");

    valueIndex++;
}
