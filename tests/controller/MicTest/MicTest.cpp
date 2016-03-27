#include "MicTest.h"
#include "Ema.h"

#define AR_PIN 14
#define MIC_PIN 15
#define GAIN_PIN 16

const int sampleWindow = 30; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
Ema maxEma (20);

void setup()
{
    Serial.begin(9600);

    pinMode(MIC_PIN, INPUT);
    pinMode(GAIN_PIN, OUTPUT);
    digitalWrite(GAIN_PIN, HIGH);
}

const int msgLength = 128;
char msg[msgLength];
int msgIndex = 0;
void processInput()
{
    if (Serial.available())
    {
        msg[msgIndex++] = (char)Serial.read();

        if (msg[msgIndex - 1] == '\r')
        {
        	Serial.print("Got : "); Serial.println(msg);

        	if (strncmp(msg, "50", 2) == 0)
        	{
        		Serial.println("========================== setting 50");
				pinMode(GAIN_PIN, OUTPUT);
        		digitalWrite(GAIN_PIN, LOW);
        	}
        	else if (strncmp(msg, "40", 2) == 0)
        	{
        		Serial.println("========================== setting 40");
				pinMode(GAIN_PIN, OUTPUT);
        		digitalWrite(GAIN_PIN, HIGH);
        	}
        	else
        	{
        		Serial.println("========================== setting 60");
				pinMode(GAIN_PIN, INPUT);
        	}

            memset(msg, '\0', msgIndex - 1);
            msgIndex = 0;
        }
    }
}

double nMinus2 = 0;
double nMinus1 = 0;
unsigned long lastMark = millis();
float LPF_Beta = 0.025;
float smoothData;
void react()
{
    unsigned long startMillis = millis();  // Start of sample window
    unsigned int peakToPeak = 0;   // peak-to-peak level

    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;

    // collect data for 50 mS
    while (millis() - startMillis < sampleWindow)
    {
        sample = analogRead(MIC_PIN);
        if (sample < 1024)  // toss out spurious readings
        {
        	if (sample > signalMax)
                signalMax = sample;  // save just the max levels
            else if (sample < signalMin)
                signalMin = sample;  // save just the min levels
        }
    }

    peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
    double volts = (peakToPeak * 3.3) / 1024;  // convert to volts
//    Serial.print(volts); Serial.print(" - ");
    smoothData = smoothData - (LPF_Beta * (smoothData - volts));
    volts = volts * 100;

    volts = map(volts, (double)0, (double)200, (double)0, (double)700) / (double) 100;

//    Serial.print(volts); Serial.print(" - ");
    double volume = pow((double)2, volts);
//    Serial.println(volume);

    if (nMinus1 > maxEma.getCurrentValue() || !maxEma.getInitialized() || ((millis() - lastMark) > 1000))
    {
        maxEma.mark((double) nMinus1 * 1.5);
        lastMark = millis();
    }

    for (int i = 0; i < volume || i < maxEma.getCurrentValue(); i++)
    {
    	if (i == (int)maxEma.getCurrentValue())
    		Serial.print("|");
    	else if (i < volume)
			Serial.print("-");
    	else
    		Serial.print(" ");
    }
    Serial.println();

    for (int i = 0; i < smoothData; i++)
    	Serial.print(".");
    Serial.println();


    nMinus2 = nMinus1;
    nMinus1 = volume;
}

void loop()
{
	processInput();
	react();
}
