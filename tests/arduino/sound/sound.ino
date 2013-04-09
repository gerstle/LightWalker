#include <SPI.h>
#include <TCL.h>

int LeftPin = 0;   // read left channel from analog input 0
int RightPin = 1;  // read Right channel from analog input 1
int strobePin = 4; // strobe is attached to digital pin 2
int resetPin = 5;  // reset is attached to digital pin 3
int spectrumValue[7]; // to hold a2d values
#define noiseFilter 50           // Noise level to ignore on EQ reading (0-1023). 100 or less is typical

const int valueCount = 100;
int values[valueCount];
int valueIndex = 0;
double valueTotal = 0;
int valueAvg = 0;

int pixel_count = 50;

void setup() 
{ 
    Serial.begin(9600);
  
    pinMode(LeftPin, INPUT);
    pinMode(RightPin, INPUT);
    pinMode(strobePin, OUTPUT);
    pinMode(resetPin, OUTPUT);
    analogReference(DEFAULT);

    digitalWrite(resetPin, LOW);
    digitalWrite(strobePin, HIGH);
    

    TCL.begin();
    TCL.sendEmptyFrame();

    digitalWrite(resetPin, HIGH);
    digitalWrite(resetPin, LOW);
    for (valueIndex = 0; valueIndex < valueCount; valueIndex++)
    {
        for (int q = 0; q < 7; q++)
        {
            digitalWrite(strobePin, LOW);
            delayMicroseconds(30); // to allow the output to settle
            spectrumValue[q] = analogRead(LeftPin);
            digitalWrite(strobePin, HIGH);
        }
        values[valueIndex] = spectrumValue[0];
        valueTotal += values[valueIndex];
    }
    
    valueAvg = valueTotal / valueCount;
}

void loop() 
{ 
    digitalWrite(resetPin, HIGH);
    digitalWrite(resetPin, LOW);

    for (int q = 0; q < 7; q++)
    {
        digitalWrite(strobePin, LOW);
        delayMicroseconds(30); // to allow the output to settle
        spectrumValue[q] = analogRead(LeftPin);
        digitalWrite(strobePin, HIGH);
//         Serial.print(spectrumValue[q]);
//         Serial.print("\t\t");
    }
//     Serial.println("");

    if (valueIndex >= valueCount)
        valueIndex = 0;

    Serial.print(spectrumValue[0]); Serial.print("\t");
    valueTotal = valueTotal - values[valueIndex] + spectrumValue[0];
    values[valueIndex] = spectrumValue[0];
    valueAvg = valueTotal / valueCount;
    Serial.print(valueAvg); Serial.print("\t");

    if (spectrumValue[0] < noiseFilter)
        spectrumValue[0] = 0;

    //float brightness = ((float)map(valueAvg, 56, 450, 0, 100)) / 100;
//     float brightness = ((float)map(spectrumValue[0], 0, valueAvg, 0, 100))/100;
    if (valueAvg > 0)
    {
        float brightness = (float)spectrumValue[0] / ((float)valueAvg * 2.5);
        Serial.println(brightness);
        int color = byte(((float) 255 * (brightness * brightness * brightness)));

        if (color > 255)
            color = 255;
        //Serial.println(valueAvg);

        TCL.sendEmptyFrame();
        for (int i = 0; i < pixel_count; i++)
            TCL.sendColor(color, color, color);
    }

    valueIndex++;

//     Serial.print(spectrumValue[0]); Serial.print("\t\t"); Serial.println(brightness);
}
